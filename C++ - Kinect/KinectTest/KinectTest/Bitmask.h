#ifndef JSS_BITMASK_HPP
#define JSS_BITMASK_HPP

// (C) Copyright 2015 Just Software Solutions Ltd
//
// Distributed under the Boost Software License, Version 1.0.
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or
// organization obtaining a copy of the software and accompanying
// documentation covered by this license (the "Software") to use,
// reproduce, display, distribute, execute, and transmit the
// Software, and to prepare derivative works of the Software, and
// to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire
// statement, including the above license grant, this restriction
// and the following disclaimer, must be included in all copies
// of the Software, in whole or in part, and all derivative works
// of the Software, unless such copies or derivative works are
// solely in the form of machine-executable object code generated
// by a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
// KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
// COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE
// LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN
// CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include<type_traits>

template<typename E>
struct enable_bitmask_operators {
	static const bool enable = false;
};

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
operator|(E lhs, E rhs) {
	typedef typename std::underlying_type<E>::type underlying;
	return static_cast<E>(
		static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
operator&(E lhs, E rhs) {
	typedef typename std::underlying_type<E>::type underlying;
	return static_cast<E>(
		static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
operator^(E lhs, E rhs) {
	typedef typename std::underlying_type<E>::type underlying;
	return static_cast<E>(
		static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
operator~(E lhs) {
	typedef typename std::underlying_type<E>::type underlying;
	return static_cast<E>(
		~static_cast<underlying>(lhs));
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E&>::type
operator|=(E& lhs, E rhs) {
	typedef typename std::underlying_type<E>::type underlying;
	lhs = static_cast<E>(
		static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
	return lhs;
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E&>::type
operator&=(E& lhs, E rhs) {
	typedef typename std::underlying_type<E>::type underlying;
	lhs = static_cast<E>(
		static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
	return lhs;
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable, E&>::type
operator^=(E& lhs, E rhs) {
	typedef typename std::underlying_type<E>::type underlying;
	lhs = static_cast<E>(
		static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
	return lhs;
}

#endif
