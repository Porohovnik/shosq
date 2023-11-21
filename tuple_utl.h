#ifndef TUPLE_UTL_H
#define TUPLE_UTL_H

#include <tuple>
#include <type_traits>
#include <optional>
#include <functional>
#include <algorithm>

using uint =unsigned int;

namespace tutl {

    template<typename T,typename M>
    M& fun_void(M &m){
        return m;
    }


    template<std::size_t Begin,std::size_t End,typename T, size_t ...i>
    inline constexpr std::tuple<std::tuple_element_t<Begin+i,std::remove_pointer_t<T>>...> * get_arguments(std::index_sequence<i...>){
        return nullptr;
    };

    template<std::size_t Begin,std::size_t End,typename ...Arg>
    inline constexpr auto Get_arguments(){
        return  get_arguments<Begin,End,std::tuple<Arg...>>(std::make_index_sequence<End-Begin>{});
    }

    template<std::size_t N,typename ...Arg>
    inline constexpr auto Get_last_arguments(){
        return  Get_arguments<sizeof... (Arg)-N,sizeof... (Arg),Arg...>();
    }

    template<std::size_t N,typename ...Arg>
    inline constexpr auto Get_first_arguments(){
        return  Get_arguments<0,N,Arg...>();
    }

    template<int N,char const t[]>
    constexpr int to_zero(){
        if constexpr (t[N]=='\0'){
            return N;
        }else {
            return to_zero<N+1,t>();
        }
    }

    template<char const t[],size_t ...i>
    constexpr auto char_to_char(std::index_sequence<i...>){
        constexpr char m[sizeof... (i)]={t[i]...};
        return m;
    }


    template<char const t[]>
    constexpr auto Char_to_char(){
        return char_to_char(t,std::make_index_sequence<to_zero<0,t>()>());
    }

    template<size_t ...i>
    constexpr auto char_to_array(const char t[],std::index_sequence<i...>){
        return std::array<char,sizeof... (i)>{t[i]...};
    }



    template<char const t[]>
    constexpr auto Char_to_array(){
        return char_to_array<t>(std::make_index_sequence<to_zero<0,t>()>());
    }

    template<std::size_t N>
    constexpr auto Char_to_array(const char t[]){
        return char_to_array(t,std::make_index_sequence<N>());
    }



    template<std::size_t N>
    static constexpr std::size_t day_on_month(std::size_t day){
        std::size_t day_on_mounthe[12]={31,29,31,30,31,30,31,31,30,31,30,31};
        if constexpr(N==366){
            day_on_mounthe[1]=29;
        }

        std::size_t k=0;
        for (std::size_t i=0;i<12;i++) {
            k+=day_on_mounthe[i];
            if(day<k){
                return i;
            }
        }
        return day;
    }

    template<typename F,size_t ...i>
    static constexpr auto to_months(F f,std::index_sequence<i...>){
        return std::array<std::size_t,sizeof...(i)>{f(i)...};
    }
    template<std::size_t N,typename F>
    static constexpr std::array<std::size_t,N> To_months(F f){
        return to_months(f,std::make_index_sequence<N>());
    }

    inline static std::array<std::size_t,365> months=To_months<365>(day_on_month<365>);
    inline static std::array<std::size_t,13> day_to_months={0,31,59,90,120,151,181,212,243,270,304,334,365};

    inline static std::size_t to_month(std::size_t day){
        return months[day];
    }
    inline static std::size_t to_day(std::size_t month){
        return day_to_months[month];
    }
    //

    template <std::size_t size,typename T,size_t... i>
    constexpr inline int arrayDublicate(T S,std::array<T,size> t,std::index_sequence<i...>) {
        int k=0;
        (((t[i]==S)?k++:false),...);
        return k;
    }

    template <std::size_t size,typename T>
    constexpr inline int ArrayDublicate(T S,std::array<T,size>  t) {
        return arrayDublicate(S,t,std::make_index_sequence<size>());
    }

    template <typename T,size_t... i>
    constexpr inline int ischeck_dublicate_to_array(T t,std::index_sequence<i...>){
        int k=0;
        ((((ArrayDublicate(t[i],t))>1)?k++:false),...);
        return k;
    }

    template <std::size_t size,typename T>
    constexpr inline bool isCheck_Dublicate_to_Array(std::array<T,size>  t) {
        return ischeck_dublicate_to_array(t,std::make_index_sequence<size>());
    }

    template <auto S, typename T,size_t... i>
    constexpr inline int arraySearch(T t,std::index_sequence<i...>) {
        int k=-1;
        (((t[i]==S)?k=i:false),...);
        return k;
    }

    template <auto S,std::size_t size,typename T>
    constexpr inline int ArraySearch(std::array<T,size>  t) {
        return arraySearch<S>(t,std::make_index_sequence<size>());
    }

    template <std::size_t size,typename T,size_t... i>
    constexpr inline bool ischeck_dublicate_to_array(std::array<T,size>  t,std::index_sequence<i...>) {
        int k=0;
        (((ArraySearch<t[i]>(t)==0)?true:k++),...);
        return k;
    }

    template <std::size_t size,typename T,size_t... i>
    constexpr inline bool isCheck_Dublicate_to_Array(std::array<T,size>  t) {
        return ischeck_dublicate_to_array(t,std::make_index_sequence<size>());
    }

    template <typename T,typename F, size_t... i>
    inline auto arrayTransform(T&& t,F &&f, std::index_sequence<i...>) {
        return std::array<decltype (f(std::forward<T>(t)[0])), sizeof... (i)>{f(std::forward<T>(t)[i])...};
    }

    template <typename T,std::size_t size,typename F>
    inline auto ArrayTransform(std::array<T, size> && t,F &&f) {
        return arrayTransform(std::forward<std::array<T, size>>(t),std::forward<F>(f),std::make_index_sequence<size>());
    }

    template <typename T,typename F, size_t... i>
    inline auto arrayArg(T&& t,F &&f, std::index_sequence<i...>) {
        return f(std::forward<T>(t)[i]...);
    }

    template <typename T,std::size_t size,typename F>
    inline auto ArrayArg(std::array<T, size> && t,F &&f) {
        return arrayArg(std::forward<std::array<T, size>>(t),std::forward<F>(f),std::make_index_sequence<size>());
    }

    template <typename T,typename F, size_t... i>
    constexpr inline void tupleForeach(T&& t,F &&f, std::index_sequence<i...>) {
        ((f(std::get<i>(std::forward<T>(t)))),...);
    }

    template <typename T,typename F>
    constexpr inline void TupleForeach(T&& t,F &&f) {
        tupleForeach(std::forward<T>(t),std::forward<F>(f),std::make_index_sequence<std::tuple_size<std::remove_reference_t<T>>::value>());
    }

    template <typename T,typename S, size_t... i>
    constexpr inline int get_tuple_type_namber(std::index_sequence<i...>) noexcept{
       int k=-1;
       int j=-1;
       auto t=std::make_tuple((std::is_same<std::tuple_element_t<i,std::remove_reference_t<T>>,S>())...);
       TupleForeach(t,[&k,&j](bool t){j++;(t==1)?k=j:k=k;});
       return k;
    }



    template <typename T,typename S>
    constexpr inline int Get_tuple_type_namber() noexcept{
        return get_tuple_type_namber<T,S>(std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<T>>>());
    }

    template<typename Type,typename T,typename K>
    constexpr auto * Get_tuple_type_element(K &s){
        constexpr int id_element=Get_tuple_type_namber<T,Type>();
        if constexpr(id_element<0){
            static_assert (id_element<0,";Error_tuple;" );
            int * t=nullptr;
            return t;
        }else{
            return &std::get<id_element>(s);
        }
    }

    template<typename Type,typename T>
    constexpr auto * Get_tuple_type_element_t(T &s){
        constexpr int id_element=Get_tuple_type_namber<std::remove_reference_t<T>,Type>();
        if constexpr(id_element<0){
            static_assert (id_element<0,";Error_tuple;" );
            int * t=nullptr;
            return t;
        }else{
            return &std::get<id_element>(s);
        }
    }


    template <typename T,auto type, size_t... i>
    constexpr inline int get_tuple_type_namber(std::index_sequence<i...>) noexcept{
       int k=-1;
       int j=-1;
       auto t=std::make_tuple((std::tuple_element_t<i,std::remove_reference_t<T>>::type==type)...);
       TupleForeach(t,[&k,&j](bool t){j++;(t==1)?k=j:k=k;});
       return k;
    }
    template <typename T,auto type>
    constexpr inline int Get_tuple_type_namber() noexcept{
        return get_tuple_type_namber<T,type>(std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<T>>>());
    }

    template<auto type,typename T,typename K>
    constexpr auto * Get_tuple_type_element(K &s){
        constexpr int id_element=Get_tuple_type_namber<T,type>();
        if constexpr(id_element<0){
            static_assert (id_element<0,";Error_tuple;" );
            int * t=nullptr;
            return t;
        }else{
            return &std::get<id_element>(s);
        }
    }



    template<typename Type,typename T>
    constexpr bool isGet_tuple_type_element(){
        constexpr int id_element=Get_tuple_type_namber<T,Type>();
        if constexpr(id_element<0){
            return false;
        }
        return true;
    }

    template<typename Type,typename S,typename  Args,typename Data, typename F>
    constexpr void Construction_serch_and_active(Data &&t,F &&f){
        if constexpr(std::is_constructible_v<Type,S>){
            std::forward<F>(f)(*Get_tuple_type_element<Type,Args>(std::forward<Data>(t)));
        }
    }

    template<typename T>
    constexpr T s(size_t i){
        return {};
    };

    template<typename T,size_t ...i>
    constexpr auto _create_tuple(std::index_sequence<i...>){
        return std::tuple_cat(std::make_tuple(s<T>(i))...);
    }
    template<int N,typename T>
    constexpr auto create_tuple(){
        return _create_tuple<T>(std::make_index_sequence<N>());
    }

    template<typename T>
    constexpr T s(T t,size_t i){
        return t;
    };

    template<typename ...T,size_t ...i>
    constexpr auto _create_tuple(std::tuple<T...> t,std::index_sequence<i...>){
        return std::tuple_cat(s(t,i)...);
    }
    template<int N,typename ...T>
    constexpr auto create_tuple(T ...t){
        auto tt=std::tuple_cat(std::make_tuple(t...));
        if constexpr(N<0){
            return std::tuple<>{};
        }else{
            return _create_tuple(tt,std::make_index_sequence<N>());
        }
    }

    template<typename J,typename ...T,size_t ...i>
    constexpr auto tuple_to_array(std::tuple<T...> t,std::index_sequence<i...>){
        return std::array<J,sizeof...(T)>{std::get<i>(t)...};
    }
    template<typename J,typename ...T>
    constexpr auto Tuple_to_array(std::tuple<T...> t){
        return tuple_to_array<J>(t,std::make_index_sequence<sizeof...(T)>());
    }
    template<size_t ...i>
    constexpr auto make_index_to_array(std::index_sequence<i...>){
        return std::array<int,sizeof...(i)>{i...};
    }
    template<int N>
    constexpr auto Make_index_to_array(){
        if constexpr(N<0){
            return std::tuple<>{};
        }else{
            return make_index_to_array(std::make_index_sequence<N>());
        }
    }


    template<typename T>
    constexpr auto Type_to(T value){
        return value;
    }

    template < template <typename...> class Template, typename T >
    struct is_instantiation_of_tuple : std::false_type {};

    template < template <typename...> class Template, typename... Args >
    struct is_instantiation_of_tuple< Template, Template<Args...> > : std::true_type {};


    template<typename T>
    constexpr auto Type_to_tuple(T value){
        if constexpr(is_instantiation_of_tuple<std::tuple,decltype (Type_to<T>({}))>()){
            return Type_to(value);
        }else{
            return std::tuple<T>(value);
        }
    }


    template<typename T>
    uint constexpr _cout_arg(){
        if constexpr(is_instantiation_of_tuple<std::tuple,T>()){
            return std::tuple_size<T>();
        }else{
            return 1;
        }
    }

    template<typename ...Arg>
    constexpr uint cout_arg(){
        uint cout_arg__=sizeof...(Arg);
        ((cout_arg__+=_cout_arg<decltype(Type_to(Arg{}))>()-1),...);
        return  cout_arg__;
    }

    template<typename ...Arg>
    constexpr auto type_disclosure(){
        return  std::tuple_cat(Type_to_tuple<Arg>({})...);
    }
    template<typename ...Arg>
    constexpr auto type_disclosure(Arg...arg){
        return  std::tuple_cat(Type_to_tuple<Arg>(arg)...);
    }



    template<auto indef,auto data_>
    struct KAD{//Key_And_Data
        static constexpr decltype (indef) type=indef;
        static constexpr decltype (data_) data=data_;
    };

    template<auto indef,typename Data>
    struct KAAT{//Key_And_Atribute_Type
        static constexpr decltype (indef) type=indef;
        std::optional<Data> data;

        void inline set_data(Data  data_){
            data.emplace( data_ );
        }
    };

    template<typename T,typename Data>
    struct TAAT{//Type_And_Atribute_Type
        using type=T;
        static T Type();
        std::optional<Data> data;

        void inline set_data(Data & data_){
            data.emplace( data_ );
        }
    };


    template <typename ...Arg>
    class Type_to_data{
        std::tuple<Arg...> types{};
    public:
        Type_to_data(){}

        template<typename F>
        Type_to_data(F &f):types(fun_void<Arg>(f())...){}

        template<auto type,typename T>
        constexpr void set_data(T data_){
            TupleForeach(types,[ &data_](auto & t){
                if constexpr(std::remove_reference_t<decltype (t)>::type==type){
                    t.set_data(data_);
                }
            });
        }

        template<typename Type,typename T>
        constexpr void set_data(T data_){
            TupleForeach(types,[ &data_](auto &t){
                if constexpr(std::is_same<typename std::remove_reference_t<decltype (t)>::type,Type>()){
                    t.set_data(data_);
                }
            });
        }


        template<auto type>
        inline constexpr auto * get_element(){
            return Get_tuple_type_element<type,std::tuple<Arg...>>(types);
        }
        template<auto type>
        inline constexpr auto & get_element_data(){
            return *Get_tuple_type_element<type,std::tuple<Arg...>>(types)->data;
        }

        template<typename Type>
        constexpr auto * get_element(){
           return Get_tuple_type_element<std::remove_reference_t<Type>,std::tuple<decltype(Arg::Type()) ...>>(types);
        }
        template<typename Type>
        constexpr auto & get_element_data(){
           return *Get_tuple_type_element<std::remove_reference_t<Type>,std::tuple<decltype(Arg::Type()) ...>>(types)->data;
        }

        constexpr auto & get_data(){ return types;}
    };

    template<typename Array,typename ...Arg>
    inline void array_to_for(Array & array,Arg &...arg){//синтаксический сахар
        std::for_each(array.begin(),array.end(),[&arg...](auto &t){t(arg...);});
    }


    template<typename ...Arg>
    inline std::function<void(Arg ...)>  get_to_for_array_fun(std::vector<std::function<void(Arg ...)>>  & array){//синтаксический сахар
        return [array](Arg ...arg){array_to_for(array,arg...);};
    }


}
#endif // TUPLE_UTL_H
