// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   2018-10-24

#ifndef _SESSION_H_
#define _SESSION_H_

#include <wrapper/Wrapper.hpp>
#include <node/Node.h>

namespace wrapper {

    /**
     * Custom handler for guarding node access.
     */
    template <class T>
    class Session : public wrapper::ExecutionHandler<T> {

    public:

        Session(T* value): wrapper::ExecutionHandler<T>(value), mutex() { }

        virtual ~Session() = default;

        /**
         * Create new session.
         * @return
         */
        static Wrapper<T, Session> createSession(T* content, bool handleMemory=false) {
            return Wrapper<T, Session> (content, new Session(node), handleMemory);
        }

        /**
         * Manages access rights, executed prior to any function call to the
         * object.
         *
         * @return  whether the access rights are currently belonging to the
         *          thread that is currently executing or not.
         */
        virtual bool prefix() {
            mutex.lock();
            return true;
        }

        /**
         * Manages access rights, executed prior to any function call to the
         * object.
         */
        virtual void suffix() {
            mutex.unlock();
        }

        /**
         * Code to be executed when the last reference to the #Node_session is
         * lost.
         */
        virtual void onDestroy() {}

    private:
        /**
         * Mutex on the element.
         */
        std::recursive_mutex mutex;
    };
}

#endif //_SESSION_H_
