#pragma once

#include <csetjmp>
#include <string_view>

namespace uxcept {


    template<typename try_t, typename catch_t>
    void tryCatch(try_t&& inTry, catch_t&& inCatch);

    void raise(std::string_view inError);


    namespace detail {

        struct Node {

            Node() {
                // ===== atomic ===== //
                if (startNode) {
                    mNext = startNode->mNext;
                }
                startNode = this;
                // ================== //
            }

            ~Node() {
                pop_front();
            }

            static Node* pop_front() {
                if (!startNode) {
                    return nullptr;
                }
                Node* out = startNode;
                startNode = startNode->mNext;
                return out;
            }

            static Node* front() {
                return startNode;
            }

            jmp_buf buffer;
            std::string_view error;
        private:
            Node* mNext = nullptr;
            inline static Node* startNode;
        };

    }


    template<typename try_t, typename catch_t>
    void tryCatch(try_t&& inTry, catch_t&& inCatch) {
        detail::Node node;
        if (!setjmp(node.buffer)) {
            inTry();
            node.pop_front();
        }
        else {
            auto* n = node.front();
            node.pop_front();
            inCatch(n->error);
        }
    }

    inline void raise(std::string_view inError) {
        if (auto* n = detail::Node::front()) {
            n->error = inError;
            longjmp(n->buffer, 1);
        }
    }



    //namespace detail {
/*
    struct Node {

        Node() {
            // ===== atomic ===== //
            if (startNode) {
                mNext = startNode->mNext;
            }
            startNode = this;
            // ================== //
        }

        ~Node() {
            pop_front();
        }

        static Node* pop_front() {
            if (!startNode) {
                return nullptr;
            }
            Node* out = startNode;
            startNode = startNode->mNext;
            return out;
        }

        static Node* front() {
            return startNode;
        }

        jmp_buf buffer;
        std::string_view error;
    private:
        Node* mNext = nullptr;
        inline static Node* startNode;
    };
*/
//}


}