#ifndef QUEUECONTAINER_H
#define QUEUECONTAINER_H
#include <boost/shared_array.hpp>

class   QueueContainer
{

public:
        typedef boost::shared_array<char> dataPtr;

protected:
        size_t  size_;
        dataPtr data_;

public:
        QueueContainer() {
                size_ = 0;
        }
        QueueContainer(size_t   insize, char *inptr) {
                size_ = insize;
                data_.reset(inptr);
        }
        QueueContainer(const QueueContainer &inqueue) {
                size_ = inqueue.size_;
                data_ = inqueue.data_;
        }
        ~QueueContainer() {}

        QueueContainer *reset(size_t insize, char *inptr) {
                size_ = insize;
                data_.reset(inptr);
                return this;
        }

        size_t  size() {
                return size_;
        }
        dataPtr data() {
                return data_;
        }
};
#endif  // QUEUECONTAINER_H
