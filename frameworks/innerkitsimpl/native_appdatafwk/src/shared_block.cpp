/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ashmem.h>
#include <fcntl.h>
#include <securec.h>
#include <shared_block.h>
#include <sys/mman.h>
#include <unistd.h>

#include <codecvt>
#include <iostream>

#include "string_ex.h"
#include "logger.h"

namespace OHOS {
namespace AppDataFwk {
SharedBlock::SharedBlock(const std::string &name, int ashmemFd, void *data, size_t size, bool readOnly)
    : mName(name), mAshmemFd(ashmemFd), mData(data), mSize(size), mReadOnly(readOnly)
{
    mHeader = static_cast<SharedBlockHeader *>(mData);
}

SharedBlock::~SharedBlock()
{
    ::munmap(mData, mSize);
    ::close(mAshmemFd);
}

std::u16string SharedBlock::ToUtf16(std::string str)
{
    return OHOS::Str8ToStr16(str);
}

std::string SharedBlock::ToUtf8(std::u16string str16)
{
    return OHOS::Str16ToStr8(str16);
}

int SharedBlock::CreateSharedBlock(const std::string &name, size_t size, int ashmemFd, SharedBlock **outSharedBlock)
{
    void *data = ::mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, ashmemFd, 0);
    if (data == MAP_FAILED) {
        LOG_ERROR("SharedBlock: mmap function error");
        return SHARED_BLOCK_ASHMEM_ERROR;
    }

    int ret = AshmemSetProt(ashmemFd, PROT_READ);
    if (ret < 0) {
        LOG_ERROR("SharedBlock: AshmemSetProt function error.");
        ::munmap(data, size);
        return SHARED_BLOCK_SET_PORT_ERROR;
    }

    /* readOnly */
    SharedBlock *block = new SharedBlock(name, ashmemFd, data, size, false);
    int result = block->Clear();
    if (!result) {
        LOG_DEBUG("Created a new SharedBlock: freeOffset=%{private}d, numRows=%{private}d, "
            "numColumns=%{private}d, mSize=%{private}d",
            block->mHeader->unusedOffset, block->mHeader->rowNums, block->mHeader->columnNums,
            static_cast<int>(block->mSize));
        *outSharedBlock = block;
        return SHARED_BLOCK_OK;
    }

    delete block;
    block = NULL;
    return result;
}

int SharedBlock::Create(const std::string &name, size_t size, SharedBlock **outSharedBlock)
{
    std::string ashmemName = "SharedBlock:" + name;

    sptr<Ashmem> ashmem = Ashmem::CreateAshmem(ashmemName.c_str(), size);
    if (ashmem == nullptr) {
        LOG_ERROR("SharedBlock: CreateAshmem function error.");
        return SHARED_BLOCK_ASHMEM_ERROR;
    }

    bool ret = ashmem->MapReadAndWriteAshmem();
    if (!ret) {
        LOG_ERROR("SharedBlock: MapReadAndWriteAshmem function error.");
        ashmem->CloseAshmem();
        return SHARED_BLOCK_SET_PORT_ERROR;
    }

    int result = CreateSharedBlock(name, size, ashmem->GetAshmemFd(), outSharedBlock);
    if (result == SHARED_BLOCK_OK) {
        return SHARED_BLOCK_OK;
    }
    ashmem->UnmapAshmem();
    ashmem->CloseAshmem();
    *outSharedBlock = nullptr;
    return result;
}

int SharedBlock::CreateFromParcel(Parcel *parcel, SharedBlock **outSharedBlock)
{
    std::string name = ToUtf8(parcel->ReadString16());

    int ashmemFd = parcel->ReadInt32();
    if (fcntl(ashmemFd, F_GETFD) < 0) {
        return SHARED_BLOCK_BAD_VALUE;
    }

    ssize_t size = AshmemGetSize(ashmemFd);
    if (size < 0) {
        return SHARED_BLOCK_ASHMEM_ERROR;
    }

    int result;
    int dupAshmemFd = ::dup(ashmemFd);
    if (dupAshmemFd < 0) {
        result = -errno;
    } else {
        void *data = ::mmap(NULL, size, PROT_READ, MAP_SHARED, dupAshmemFd, 0);
        if (data == MAP_FAILED) {
            LOG_ERROR("SharedBlock: mmap function failed.");
            result = -errno;
            ::close(dupAshmemFd);
        } else if (AshmemGetSize(dupAshmemFd) != size) {
            LOG_ERROR("SharedBlock: mmap function error for bad file descriptor.");
            ::munmap(data, size);
            ::close(dupAshmemFd);
            result = -errno;
        } else {
            /* readOnly */
            SharedBlock *block = new SharedBlock(name, dupAshmemFd, data, size, true);
            LOG_DEBUG("Created SharedBlock from parcel: unusedOffset=%{private}d, "
                "rowNums=%{private}d, columnNums=%{private}d, mSize=%{private}d, mData=%{private}p",
                block->mHeader->unusedOffset, block->mHeader->rowNums, block->mHeader->columnNums,
                static_cast<int>(block->mSize), block->mData);
            *outSharedBlock = block;
            return SHARED_BLOCK_OK;
        }
    }
    *outSharedBlock = nullptr;
    return result;
}

int SharedBlock::WriteToParcel(Parcel &parcel)
{
    LOG_DEBUG("To write SharedBlock to Parcel.");
    return parcel.WriteString16(ToUtf16(mName)) && parcel.WriteInt32(mAshmemFd);
}

int SharedBlock::Clear()
{
    if (mReadOnly) {
        return SHARED_BLOCK_INVALID_OPERATION;
    }

    mHeader->unusedOffset = sizeof(SharedBlockHeader) + sizeof(RowGroupHeader);
    mHeader->firstRowGroupOffset = sizeof(SharedBlockHeader);
    mHeader->rowNums = 0;
    mHeader->columnNums = 0;

    RowGroupHeader *firstGroup = static_cast<RowGroupHeader *>(OffsetToPtr(mHeader->firstRowGroupOffset));
    if (!firstGroup) {
        LOG_ERROR("Failed to get group in clear().");
        return SHARED_BLOCK_BAD_VALUE;
    }
    firstGroup->nextGroupOffset = 0;
    return SHARED_BLOCK_OK;
}

int SharedBlock::SetColumnNum(uint32_t numColumns)
{
    if (mReadOnly) {
        return SHARED_BLOCK_INVALID_OPERATION;
    }

    uint32_t cur = mHeader->columnNums;
    if ((cur > 0 || mHeader->rowNums > 0) && cur != numColumns) {
        LOG_ERROR("Trying to go from %{public}d columns to %{public}d", cur, numColumns);
        return SHARED_BLOCK_INVALID_OPERATION;
    }
    if (numColumns > COL_MAX_NUM) {
        LOG_ERROR("Trying to set %{public}d columns out of size", numColumns);
        return SHARED_BLOCK_INVALID_OPERATION;
    }
    mHeader->columnNums = numColumns;
    return SHARED_BLOCK_OK;
}

int SharedBlock::AllocRow()
{
    if (mReadOnly) {
        return SHARED_BLOCK_INVALID_OPERATION;
    }

    /* Fill in the row offset */
    uint32_t *rowOffset = AllocRowOffset();
    if (rowOffset == NULL) {
        return SHARED_BLOCK_NO_MEMORY;
    }

    /* Allocate the units for the field directory */
    size_t fieldDirSize = mHeader->columnNums * sizeof(CellUnit);

    /* Aligned */
    uint32_t fieldDirOffset = Alloc(fieldDirSize, true);
    if (!fieldDirOffset) {
        mHeader->rowNums--;
        LOG_INFO("Alloc the row failed, so back out the new row accounting from allocRowoffset %{public}d",
            mHeader->rowNums);
        return SHARED_BLOCK_NO_MEMORY;
    }

    CellUnit *fieldDir = static_cast<CellUnit *>(OffsetToPtr(fieldDirOffset));
    if (fieldDir == NULL) {
        return SHARED_BLOCK_BAD_VALUE;
    }
    int result = memset_s(fieldDir, fieldDirSize, 0, fieldDirSize);
    if (result != 0) {
        LOG_ERROR("Set memory failed");
        return SHARED_BLOCK_NO_MEMORY;
    }
    LOG_DEBUG("Allocated row %{private}u, row offset is at offset %{private}u,"
        " fieldDir is %{private}d bytes at offset %{private}u\n",
        mHeader->rowNums - 1, OffsetFromPtr(rowOffset), static_cast<int>(fieldDirSize), fieldDirOffset);

    *rowOffset = fieldDirOffset;
    return SHARED_BLOCK_OK;
}

int SharedBlock::FreeLastRow()
{
    if (mReadOnly) {
        return SHARED_BLOCK_INVALID_OPERATION;
    }

    if (mHeader->rowNums > 0) {
        mHeader->rowNums--;
    }

    return SHARED_BLOCK_OK;
}

uint32_t SharedBlock::Alloc(size_t size, bool aligned)
{
    /* Number of unused offsets in the header */
    uint32_t offsetDigit = 3;
    uint32_t padding = aligned ? (~mHeader->unusedOffset + 1) & offsetDigit : 0;
    uint32_t offset = mHeader->unusedOffset + padding;
    uint32_t nextFreeOffset;

    if (offset + size > mSize) {
        LOG_ERROR("SharedBlock is full: requested allocation %{public}zu bytes,"
            " free space %{public}zu bytes, block size %{public}zu bytes",
            size, mSize - mHeader->unusedOffset, mSize);
        return 0;
    }
    nextFreeOffset = offset + size;
    mHeader->unusedOffset = nextFreeOffset;
    return offset;
}

uint32_t *SharedBlock::GetRowOffset(uint32_t row)
{
    uint32_t rowPos = row;

    RowGroupHeader *group = static_cast<RowGroupHeader *>(OffsetToPtr(mHeader->firstRowGroupOffset));
    if (group == NULL) {
        LOG_ERROR("Failed to get group in getRowOffset().");
        return NULL;
    }

    while (rowPos >= ROW_OFFSETS_NUM) {
        group = static_cast<RowGroupHeader *>(OffsetToPtr(group->nextGroupOffset));
        if (group == NULL) {
            LOG_ERROR("Failed to get group in OffsetToPtr(group->nextGroupOffset) when while loop.");
            return NULL;
        }
        rowPos -= ROW_OFFSETS_NUM;
    }

    return &group->rowOffsets[rowPos];
}

uint32_t *SharedBlock::AllocRowOffset()
{
    uint32_t rowPos = mHeader->rowNums;

    RowGroupHeader *group = static_cast<RowGroupHeader *>(OffsetToPtr(mHeader->firstRowGroupOffset));
    if (group == NULL) {
        LOG_ERROR("Failed to get group in allocRowOffset().");
        return NULL;
    }

    while (rowPos > ROW_OFFSETS_NUM) {
        group = static_cast<RowGroupHeader *>(OffsetToPtr(group->nextGroupOffset));
        if (group == NULL) {
            LOG_ERROR("Failed to get group in OffsetToPtr(group->nextGroupOffset) when while loop.");
            return NULL;
        }
        rowPos -= ROW_OFFSETS_NUM;
    }
    if (rowPos == ROW_OFFSETS_NUM) {
        if (!group->nextGroupOffset) {
            /* Aligned */
            group->nextGroupOffset = Alloc(sizeof(RowGroupHeader), true);
            if (!group->nextGroupOffset) {
                return NULL;
            }
        }
        group = static_cast<RowGroupHeader *>(OffsetToPtr(group->nextGroupOffset));
        if (group == NULL) {
            LOG_ERROR("Failed to get group in OffsetToPtr(group->nextGroupOffset).");
            return NULL;
        }
        group->nextGroupOffset = 0;
        rowPos = 0;
    }

    mHeader->rowNums += 1;
    return &group->rowOffsets[rowPos];
}

SharedBlock::CellUnit *SharedBlock::GetCellUnit(uint32_t row, uint32_t column)
{
    if (row >= mHeader->rowNums || column >= mHeader->columnNums) {
        LOG_ERROR("Failed to read row %{public}d, column %{public}d from a SharedBlock"
            " which has %{public}d rows, %{public}d columns.",
            row, column, mHeader->rowNums, mHeader->columnNums);
        return NULL;
    }

    uint32_t *rowOffset = GetRowOffset(row);
    if (!rowOffset) {
        LOG_ERROR("Failed to find rowOffset for row %{public}d.", row);
        return NULL;
    }

    CellUnit *cellUnit = static_cast<CellUnit *>(OffsetToPtr(*rowOffset));
    if (!cellUnit) {
        LOG_ERROR("Failed to find cellUnit for rowOffset %{public}d.", *rowOffset);
        return NULL;
    }

    return &cellUnit[column];
}

int SharedBlock::PutBlob(uint32_t row, uint32_t column, const void *value, size_t size)
{
    return PutBlobOrString(row, column, value, size, CELL_UNIT_TYPE_BLOB);
}

int SharedBlock::PutString(uint32_t row, uint32_t column, const char *value, size_t sizeIncludingNull)
{
    return PutBlobOrString(row, column, value, sizeIncludingNull, CELL_UNIT_TYPE_STRING);
}

int SharedBlock::PutBlobOrString(uint32_t row, uint32_t column, const void *value, size_t size, int32_t type)
{
    if (mReadOnly) {
        return SHARED_BLOCK_INVALID_OPERATION;
    }

    CellUnit *cellUnit = GetCellUnit(row, column);
    if (!cellUnit) {
        return SHARED_BLOCK_BAD_VALUE;
    }

    uint32_t offset = Alloc(size);
    if (!offset) {
        return SHARED_BLOCK_NO_MEMORY;
    }

    void *ptr = OffsetToPtr(offset);
    if (!ptr) {
        return SHARED_BLOCK_NO_MEMORY;
    }

    if (size != 0) {
        int result = memcpy_s(ptr, size, value, size);
        if (result != 0) {
            return SHARED_BLOCK_NO_MEMORY;
        }
    }

    cellUnit->type = type;
    cellUnit->cell.stringOrBlobValue.offset = offset;
    cellUnit->cell.stringOrBlobValue.size = size;
    return SHARED_BLOCK_OK;
}

int SharedBlock::PutLong(uint32_t row, uint32_t column, int64_t value)
{
    if (mReadOnly) {
        return SHARED_BLOCK_INVALID_OPERATION;
    }

    CellUnit *cellUnit = GetCellUnit(row, column);
    if (!cellUnit) {
        return SHARED_BLOCK_BAD_VALUE;
    }

    cellUnit->type = CELL_UNIT_TYPE_INTEGER;
    cellUnit->cell.longValue = value;
    return SHARED_BLOCK_OK;
}

int SharedBlock::PutDouble(uint32_t row, uint32_t column, double value)
{
    if (mReadOnly) {
        return SHARED_BLOCK_INVALID_OPERATION;
    }

    CellUnit *cellUnit = GetCellUnit(row, column);
    if (!cellUnit) {
        return SHARED_BLOCK_BAD_VALUE;
    }

    cellUnit->type = CELL_UNIT_TYPE_FLOAT;
    cellUnit->cell.doubleValue = value;
    return SHARED_BLOCK_OK;
}

int SharedBlock::PutNull(uint32_t row, uint32_t column)
{
    if (mReadOnly) {
        return SHARED_BLOCK_INVALID_OPERATION;
    }

    CellUnit *cellUnit = GetCellUnit(row, column);
    if (!cellUnit) {
        return SHARED_BLOCK_BAD_VALUE;
    }

    cellUnit->type = CELL_UNIT_TYPE_NULL;
    cellUnit->cell.stringOrBlobValue.offset = 0;
    cellUnit->cell.stringOrBlobValue.size = 0;
    return SHARED_BLOCK_OK;
}

size_t SharedBlock::SetRawData(const void *rawData, size_t size)
{
    if (size <= 0) {
        LOG_ERROR("SharedBlock rawData is less than or equal to 0M");
        return SHARED_BLOCK_INVALID_OPERATION;
    }
    if (size > mSize) {
        LOG_ERROR("SharedBlock size is %{public}zu, current byteArray size is %{public}zu", mSize, size);
        return SHARED_BLOCK_NO_MEMORY;
    }

    int result = memcpy_s(mHeader, mSize, rawData, size);
    if (result != 0) {
        return SHARED_BLOCK_NO_MEMORY;
    }
    return SHARED_BLOCK_OK;
}

void *SharedBlock::OffsetToPtr(uint32_t offset, uint32_t bufferSize)
{
    if (offset >= mSize) {
        LOG_ERROR("Offset %" PRIu32 " out of bounds, max value %zu", offset, mSize);
        return NULL;
    }
    if (offset + bufferSize > mSize) {
        LOG_ERROR("End offset %" PRIu32 " out of bounds, max value %zu", offset + bufferSize, mSize);
        return NULL;
    }
    return static_cast<uint8_t *>(mData) + offset;
}

uint32_t SharedBlock::OffsetFromPtr(void *ptr)
{
    return static_cast<uint8_t *>(ptr) - static_cast<uint8_t *>(mData);
}
} // namespace AppDataFwk
} // namespace OHOS
