/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * MIT License                                                                     *
 *                                                                                 *
 * Copyright (c) 2024 Thomas AUBERT                                                *
 *                                                                                 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy    *
 * of this software and associated documentation files (the "Software"), to deal   *
 * in the Software without restriction, including without limitation the rights    *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is           *
 * furnished to do so, subject to the following conditions:                        *
 *                                                                                 *
 * The above copyright notice and this permission notice shall be included in all  *
 * copies or substantial portions of the Software.                                 *
 *                                                                                 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
 * SOFTWARE.                                                                       *
 *                                                                                 *
 * github : https://github.com/ThomasAUB/uxcept                                    *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include <csetjmp>

#if __has_include("uxcept_error_type.hpp")
#include "uxcept_error_type.hpp"
#else
#include <string_view>
namespace uxcept { using error_t = std::string_view; }
#endif


namespace uxcept {

    /**
     * @brief Try catch bloc.
     *
     * @tparam try_t Try lambda type.
     * @tparam catch_t Catch lambda type.
     * @param inTry Try lambda function.
     * @param inCatch Lambda function called if try failed.
     */
    template<typename try_t, typename catch_t>
    void tryCatch(try_t&& inTry, catch_t&& inCatch);

    /**
     * @brief Raises an error.
     *
     * @param inError Error value.
     */
    void raise(error_t inError);

    /**
     * @brief Asserts a condition.
     *
     * @param inCondition Condition to assert.
     * @param inError Error raised if the condition was false.
     */
    void check(bool inCondition, error_t inError);

    namespace conf {

        /**
         * @brief Set the Atomic object
         *
         * @param inEnterAtomic Enter atomic function pointer.
         * @param inExitAtomic Exit atomic function pointer.
         */
        void setAtomic(void(*inEnterAtomic)(), void(*inExitAtomic)());

        /**
         * @brief Set the error handler to call when no catch function
         * have been found.
         *
         * @param f Pointer to the function to be called.
         */
        void setDefaultErrorHandler(void(*f)(error_t));

    }





    namespace detail {

        class Node {
            static constexpr void empty() {}
            static constexpr void empty(error_t) {}
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

            static void errorHandler(error_t e) {
                defaultErrorHandler(e);
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

            static void setDefaultErrorHandler(void(*f)(error_t)) {
                if (f) {
                    defaultErrorHandler = f;
                }
                else {
                    defaultErrorHandler = empty;
                }
            }

            jmp_buf buffer {};
            error_t error = error_t();

        private:

            inline static void(*enterAtomic)() = empty;
            inline static void(*exitAtomic)() = empty;
            thread_local inline static void(*defaultErrorHandler)(error_t) = empty;
            thread_local inline static Node* startNode = nullptr;

            Node* mNext = nullptr;
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

    inline void raise(error_t inError) {
        if (auto* n = detail::Node::front()) {
            n->error = inError;
            longjmp(n->buffer, 1);
        }
        else {
            // no try-catch found : call default error handler
            detail::Node::errorHandler(inError);
        }
    }

    inline void check(bool inCondition, error_t inError) {
        if (!inCondition) {
            raise(inError);
        }
    }

    namespace conf {

        inline void setAtomic(void(*inEnterAtomic)(), void(*inExitAtomic)()) {
            detail::Node::setEnterAtomic(inEnterAtomic);
            detail::Node::setExitAtomic(inExitAtomic);
        }

        inline void setDefaultErrorHandler(void(*f)(error_t)) {
            detail::Node::setDefaultErrorHandler(f);
        }

    }

}