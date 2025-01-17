/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/commands/CopyAndReleaseBuffer.h>
#include <vsg/io/Options.h>

using namespace vsg;

CopyAndReleaseBuffer::CopyAndReleaseBuffer(ref_ptr<MemoryBufferPools> optional_stagingMemoryBufferPools) :
    stagingMemoryBufferPools(optional_stagingMemoryBufferPools)
{
}

CopyAndReleaseBuffer::~CopyAndReleaseBuffer()
{
    for (auto& copyData : completed) copyData.source.release();
    for (auto& copyData : pending) copyData.source.release();
}

void CopyAndReleaseBuffer::copy(ref_ptr<Data> data, BufferInfo dest)
{
    VkDeviceSize datalSize = data->dataSize();
    VkDeviceSize alignment = std::max(VkDeviceSize(4), VkDeviceSize(data->valueSize()));

    //std::cout<<"CopyAndReleaseImage::copyDirectly() datalSize = "<<datalSize<<std::endl;

    VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    BufferInfo stagingBufferInfo = stagingMemoryBufferPools->reserveBuffer(datalSize, alignment, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, memoryPropertyFlags);
    stagingBufferInfo.data = data;

    // std::cout<<"stagingBufferInfo.buffer "<<stagingBufferInfo.buffer.get()<<", "<<stagingBufferInfo.offset<<", "<<stagingBufferInfo.range<<")"<<std::endl;

    auto deviceID = stagingMemoryBufferPools->device->deviceID;
    ref_ptr<Buffer> imageStagingBuffer(stagingBufferInfo.buffer);
    ref_ptr<DeviceMemory> stagingMemory(imageStagingBuffer->getDeviceMemory(deviceID));

    if (!stagingMemory) return;

    // copy data to staging memory
    stagingMemory->copy(imageStagingBuffer->getMemoryOffset(deviceID) + stagingBufferInfo.offset, datalSize, data->dataPointer());

    add(stagingBufferInfo, dest);
}

void CopyAndReleaseBuffer::add(BufferInfo src, BufferInfo dest)
{
    pending.push_back(CopyData{src, dest});
}

void CopyAndReleaseBuffer::CopyData::record(CommandBuffer& commandBuffer) const
{
    //std::cout<<"CopyAndReleaseBuffer::CopyData::record(CommandBuffer& commandBuffer) source.offset = "<<source.offset<<", "<<destination.offset<<std::endl;
    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = source.offset;
    copyRegion.dstOffset = destination.offset;
    copyRegion.size = source.range;
    vkCmdCopyBuffer(commandBuffer, source.buffer->vk(commandBuffer.deviceID), destination.buffer->vk(commandBuffer.deviceID), 1, &copyRegion);
}

void CopyAndReleaseBuffer::record(CommandBuffer& commandBuffer) const
{
    for (auto& copyData : completed) copyData.source.release();

    completed.clear();

    for (auto& copyData : pending)
    {
        copyData.record(commandBuffer);
    }

    pending.swap(completed);
}
