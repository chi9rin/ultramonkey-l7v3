#ifndef ATOMIC_H
#define ATOMIC_H
#include <boost/utility.hpp>
#include "wrlock.h"

namespace l7vs
{

template<class T> class atomic : boost::noncopyable
{
protected:
        mutable volatile T p;
        wr_mutex     mutex;
public:
        explicit atomic() : p(0) {}

        T get() {
                rd_scoped_lock(mutex);
                return p;
        }
        atomic &operator=(const T &q) {
                rw_scoped_lock(mutex);
                return *this;
        }
        atomic &operator++(const int) {
                rw_scoped_lock(mutex);
                p++;
                return *this;
        }
        atomic &operator--(const int) {
                rw_scoped_lock(mutex);
                p--;
                return *this;
        }
        atomic &operator+=(const T &q) {
                rw_scoped_lock(mutex);
                p += q;
                return *this;
        }
        atomic &operator-=(const T &q) {
                rw_scoped_lock(mutex);
                p -= q;
                return *this;
        }
        bool operator<(T &q) const {
                return p < q;
        }
        bool operator<=(T &q) const {
                return p <= q;
        }
        bool operator>(T &q) const {
                return p > q;
        }
        bool operator>=(T &q) const {
                return p >= q;
        }
        bool operator==(T &q) const {
                return p == q;
        }
        bool operator!=(T &q) const {
                return p != q;
        }
};


//int
template <>
class atomic<int>
{
protected:
        mutable volatile int p;
public:
        int get() {
                return p;
        }
        atomic &operator=(const int &q) {
                __sync_lock_test_and_set(&p, q);
                return *this;
        }
        atomic &operator++(const int) {
                __sync_add_and_fetch(&p, 1);
                return *this;
        }
        atomic &operator--(const int) {
                __sync_sub_and_fetch(&p, 1);
                return *this;
        }
        atomic &operator+=(const int &q) {
                __sync_add_and_fetch(&p, q);
                return *this;
        }
        atomic &operator-=(const int &q) {
                __sync_sub_and_fetch(&p, q);
                return *this;
        }
        bool operator<(const int &q) const {
                return p < q;
        }
        bool operator<=(const int &q) const {
                return p <= q;
        }
        bool operator>(const int &q) const {
                return p > q;
        }
        bool operator>=(const int &q) const {
                return p >= q;
        }
        bool operator==(const int &q) const {
                return p == q;
        }
        bool operator!=(const int &q) const {
                return p != q;
        }
};


//longlong
template <>
class atomic<long long>
{
protected:
        mutable volatile long long p;
public:
        long long get() {
                return p;
        }
        atomic &operator=(const long long &q) {
                __sync_lock_test_and_set(&p, q);
                return *this;
        }
        atomic &operator++(const int) {
                __sync_add_and_fetch(&p, 1);
                return *this;
        }
        atomic &operator--(const int) {
                __sync_sub_and_fetch(&p, 1);
                return *this;
        }
        atomic &operator+=(const long long &q) {
                __sync_add_and_fetch(&p, q);
                return *this;
        }
        atomic &operator-=(const long long &q) {
                __sync_sub_and_fetch(&p, q);
                return *this;
        }
        bool operator<(const long long &q) const {
                return p < q;
        }
        bool operator<=(const long long &q) const {
                return p <= q;
        }
        bool operator>(const long long &q) const {
                return p > q;
        }
        bool operator>=(const long long &q) const {
                return p >= q;
        }
        bool operator==(const long long &q) const {
                return p == q;
        }
        bool operator!=(const long long &q) const {
                return p != q;
        }
};

//unsigned longlong
template <>
class atomic<unsigned long long>
{
protected:
        mutable volatile unsigned long long  p;
public:
        unsigned long long get() {
                return p;
        }
        atomic &operator=(const unsigned long long &q) {
                __sync_lock_test_and_set(&p, q);
                return *this;
        }
        atomic &operator++(const int) {
                __sync_add_and_fetch(&p, 1);
                return *this;
        }
        atomic &operator--(const int) {
                __sync_sub_and_fetch(&p, 1);
                return *this;
        }
        atomic &operator+=(const unsigned long long &q) {
                __sync_add_and_fetch(&p, q);
                return *this;
        }
        atomic &operator-=(const unsigned long long &q) {
                __sync_sub_and_fetch(&p, q);
                return *this;
        }
        bool operator<(const unsigned long long &q) const {
                return p < q;
        }
        bool operator<=(const unsigned long long &q) const {
                return p <= q;
        }
        bool operator>(const unsigned long long &q) const {
                return p > q;
        }
        bool operator>=(const unsigned long long &q) const {
                return p >= q;
        }
        bool operator==(const unsigned long long &q) const {
                return p == q;
        }
        bool operator!=(const unsigned long long &q) const {
                return p != q;
        }
};

} //namespace l7vsd
#endif
