#pragma once

#include <csetjmp>
#include "uxcept_error_type.hpp"

namespace uxcept {

    template<typename try_t, typename catch_t>
    void tryCatch(try_t&& inTry, catch_t&& inCatch);

    void raise(error_t inError);

    namespace conf {
        void setEnterAtomic(void(*f)());
        void setExitAtomic(void(*f)());
    }

    namespace detail {

        class Node {
            static constexpr void empty() {}
        public:

            Node() {
                enterAtomic();
                mNext = startNode;
                startNode = this;
                exitAtomic();
            }

            static void pop_front() {
                enterAtomic();
                if (startNode) {
                    startNode = startNode->mNext;
                }
                exitAtomic();
            }

            static Node* front() {
                return startNode;
            }

            static void setEnterAtomic(void(*f)()) {
                if (f) {
                    enterAtomic = f;
                }
                else {
                    enterAtomic = empty;
                }
            }

            static void setExitAtomic(void(*f)()) {
                if (f) {
                    exitAtomic = f;
                }
                else {
                    exitAtomic = empty;
                }
            }

            jmp_buf buffer;
            error_t error;

        private:

            inline static void(*enterAtomic)() = empty;
            inline static void(*exitAtomic)() = empty;

            Node* mNext = nullptr;
            thread_local inline static Node* startNode;
        };

    }


    template<typename try_t, typename catch_t>
    void tryCatch(try_t&& inTry, catch_t&& inCatch) {
#if !defined(UEXCEPT_HARD_FAIL)
        detail::Node node;
        if (!setjmp(node.buffer)) {
            inTry();
            detail::Node::pop_front();
        }
        else {
            detail::Node::pop_front();
            inCatch(node.error);
        }
#endif
    }

    inline void raise(error_t inError) {
        if (auto* n = detail::Node::front()) {
            n->error = inError;
            longjmp(n->buffer, 1);
        }
        else {
            while (true);
        }
    }

    namespace conf {

        inline void setEnterAtomic(void(*f)()) {
            detail::Node::setEnterAtomic(f);
        }

        inline void setExitAtomic(void(*f)()) {
            detail::Node::setExitAtomic(f);
        }

    }

}