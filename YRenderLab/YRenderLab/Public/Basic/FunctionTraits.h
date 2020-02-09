#ifndef _YRENDER_BASIC_FUNCTIONTRAITS_H_	
#define _YRENDER_BASIC_FUNCTIONTRAITS_H_

#include <tuple>
// src
// https://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda

namespace LambdaHelper {

	template<typename T>
	struct FunctionTraits;


	template<typename LambdaType>
	struct FunctionTraitsLambda :
		public FunctionTraits<decltype(&LambdaType::operator())>
	{

	};

	//Member Function type
	//Ĭ�ϵ�lambda����const�޶�������constƥ�䲻����Ӧģ��
	template<typename ClassType, typename ReturnType, typename ... Args>
	struct FunctionTraits<ReturnType(ClassType::*)(Args...) const> {
		enum { ArgsNum = sizeof...(Args) };

		typedef ReturnType result_type; 

		typedef ClassType class_type;

		template<size_t i>
		struct ArgsType {
			using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
		};
	};


	//Ordinary Function type
	template<typename ReturnType, typename ... Args>
	struct FunctionTraits<ReturnType(Args...)> {
		enum { ArgsNum = sizeof...(Args) };

		using result_type = ReturnType;

		template<size_t i>
		struct ArgsType {
			using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
		};
	};
}

//����iΪ3ʱ����һֱ������tuple_element<3, tuple<_This, _Rest...>>
//����type = _This�Ӷ��ó�����

//template<size_t _Index>
//struct tuple_element<_Index, tuple<>>
//{	// enforce bounds checking
//	static_assert(_Always_false<integral_constant<size_t, _Index>>,
//		"tuple index out of bounds");
//};
//
//template<class _This,
//	class... _Rest>
//	struct tuple_element<0, tuple<_This, _Rest...>>
//{	// select first element
//	using type = _This;
//	using _Ttype = tuple<_This, _Rest...>;
//};
//
//template<size_t _Index,
//	class _This,
//	class... _Rest>
//	struct tuple_element<_Index, tuple<_This, _Rest...>>
//	: public tuple_element<_Index - 1, tuple<_Rest...>>
//{	// recursive tuple_element definition
//};

#endif