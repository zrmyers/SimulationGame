#pragma once

#include <vector>

namespace Graphics {
    // Common Interface for upload data.
    class IGPUDataUpload {

        public:
            IGPUDataUpload() = default;
            IGPUDataUpload(const IGPUDataUpload&) = default;
            IGPUDataUpload(IGPUDataUpload&&) = default;
            IGPUDataUpload& operator=(const IGPUDataUpload&) = default;
            IGPUDataUpload& operator=(IGPUDataUpload&&) = default;
            virtual ~IGPUDataUpload() = default;

            // Return total number of bytes of data
            virtual size_t GetDataSize() = 0;
            virtual void* GetDataPtr() = 0;
    };

    //! Simple buffer upload data.
    template<typename T>
    class GPUDataUpload : public IGPUDataUpload {

        public:
            GPUDataUpload(std::vector<T>&& data) {
                m_data(std::move(data));
            }

            size_t GetDataSize() const {
                return m_data.size() * sizeof(T);
            }

            void* GetDataPtr() const {
                return static_cast<void*>(m_data.data());
            }

        private:
            std::vector<T> m_data;
    };

    class GPUDataUploadZeroCopy : public IGPUDataUpload {

        public:
            GPUDataUploadZeroCopy(void* p_data, size_t data_len)
                : m_p_data(p_data)
                , m_data_len(data_len) {
            }

            size_t GetDataSize() const {
                return m_data_len;
            }

            void* GetDataPtr() const {
                return m_p_data;
            }

        private:
            void* m_p_data;
            size_t m_data_len;
    };
}