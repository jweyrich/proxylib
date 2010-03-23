#pragma once

#include <set>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace proxylib {

template<class T>
class shared_manager : private boost::noncopyable {
	public:
		typedef T								type;
		typedef boost::shared_ptr<type>			shared_type;
		typedef std::set<shared_type>			container;
		typedef typename container::iterator	iterator;

		void add(shared_type item) {
			_container.insert(item);
		}
		void remove(shared_type item) {
			_container.erase(item);
		}
		void clear() {
			_container.clear();
		}
		iterator begin() const {
			return _container.begin();
		}
		iterator end() const {
			return _container.end();
		}

	private:
		container	_container;
};

} // namespace proxylib
