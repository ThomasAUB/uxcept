#pragma once

#include <csetjmp>
#include <string_view>

namespace uxcept {

    template<typename try_t, typename catch_t>
    void tryCatch(try_t&& inTry, catch_t&& inCatch);

    void raise(std::string_view inError);

    namespace conf {
        void setEnterAtomic(void(*f)());
        void setExitAtomic(void(*f)());
    }

    namespace detail {

        class Node {
            static void empty() {}
        public:

            Node() {
                enterAtomic();
                if (startNode) {
                    mNext = startNode;
                }
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

            inline static void(*enterAtomic)() = empty;
            inline static void(*exitAtomic)() = empty;

            jmp_buf buffer;
            std::string_view error;

        private:
            Node* mNext = nullptr;
            thread_local inline static Node* startNode;
        };

    }


    template<typename try_t, typename catch_t>
    void tryCatch(try_t&& inTry, catch_t&& inCatch) {
        detail::Node node;
        if (!setjmp(node.buffer)) {
            inTry();
            detail::Node::pop_front();
        }
        else {
            detail::Node::pop_front();
            inCatch(node.error);
        }
    }

    inline void raise(std::string_view inError) {
        if (auto* n = detail::Node::front()) {
            n->error = inError;
            longjmp(n->buffer, 1);
        }
    }

    namespace conf {

        inline void setEnterAtomic(void(*f)()) {
            detail::Node::enterAtomic = f;
        }

        inline void setExitAtomic(void(*f)()) {
            detail::Node::exitAtomic = f;
        }

    }

}