#ifndef DOWNLOAD_MANAGEMENT_H
#define DOWNLOAD_MANAGEMENT_H

#include <sqlite3.h>
#include <tuple_utl.h>

#include <vector>
#include <array>
#include <algorithm>
#include <iostream>
#include <map>
#include <cassert>

inline int tutl_LOG_REQUEST_TO_CONSOLE=1;

namespace tutl {
using namespace tutl;

template <typename T>
constexpr uint defining_the_type(){
    if constexpr(std::is_same<T,bool>()){
        return SQLITE_INTEGER;
    }
    if constexpr(std::is_same<T,int>()){
        return SQLITE_INTEGER;
    }
    if constexpr(std::is_same<T,sqlite3_int64>()){
        return SQLITE_INTEGER;
    }
    if constexpr(std::is_same<T,float>()){
        return SQLITE_FLOAT;
    }
    if constexpr(std::is_same<T,double>()){
        return SQLITE_FLOAT;
    }
    if constexpr(std::is_same<T,char *>()){
        return SQLITE_TEXT;
    }
    if constexpr(std::is_same<T,std::string>()){
        return SQLITE_TEXT;
    }
    if constexpr(std::is_same<T,wchar_t *>()){
        return SQLITE_TEXT;
    }
    if constexpr(std::is_same<T,std::wstring>()){
        return SQLITE_TEXT;
    }
    return SQLITE_BLOB;
}

template<int i=0,int N,typename T,typename J,typename ...Arg>
constexpr void tupleForeachDefining(std::array<T,N>&array){
    array[i]=defining_the_type<J>();
    if constexpr(i!=(N-1)){
        tupleForeachDefining<i+1,N,T,Arg...>(array);
    }
}

template<int i=0,int N,typename T,typename ...Arg>
constexpr void TupleForeachDefining(std::tuple<Arg...> t,std::array<T,N>&array){
    tupleForeachDefining<0,N,T,Arg...>(array);
}

class Stmt{
    sqlite3_stmt * program=nullptr;
    int current_id=1;
    bool incelezation_on=false;
public:
    constexpr Stmt(){};
    Stmt(sqlite3 * db,std::string & sql_request,uint flag_=0){
        //std::cout<<sql_request.data()<<std::endl;
        int err=sqlite3_prepare_v2(db,sql_request.data(),-1,&program,nullptr);
        if(err!=SQLITE_OK){
            std::cout<<"Error: "<<err<<" db: "<<sqlite3_errmsg(db)<<std::endl;
        }else{
            incelezation_on=true;
        }
    };

    ~Stmt(){
       if(incelezation_on){
           if (program!=nullptr){
               sqlite3_reset(program);
               sqlite3_finalize(program);
           }
       }
    }
    int reset(){
        return sqlite3_reset(program);
    }

    int use(){
        int s=sqlite3_step(program);
        current_id=1;
        return s;
    }

    template<typename T>
    inline int Bind(T &&value){
        if constexpr(is_instantiation_of_tuple<std::tuple,T>()){
            TupleForeach(std::forward<T>(value),[this](auto value){Bind(Type_to(value));});
            return 0;
        }
        return sqlite3_bind_blob(program,current_id++,&value,sizeof(value),nullptr);
    }

    inline int Bind(){
        return sqlite3_bind_null(program,current_id++);
    }

    inline int Bind(sqlite3_value* &&value){
        return sqlite3_bind_value(program,current_id++,std::forward<sqlite3_value*>(value));
    }
    inline int Bind(bool &&value){
        return sqlite3_bind_int(program,current_id++,std::forward<bool>(value));
    }
    inline int Bind(int &&value){
        return sqlite3_bind_int(program,current_id++,std::forward<int>(value));
    }
    inline int Bind(float &&value){
        return sqlite3_bind_double(program,current_id++,static_cast<float>(std::forward<float>(value)));
    }
    inline int Bind(double &&value){
        return sqlite3_bind_double(program,current_id++,std::forward<double>(value));
    }

    inline int Bind(sqlite3_int64 &&value){
        return sqlite3_bind_int64(program,current_id++,std::forward<sqlite3_int64>(value));
    }
    inline int Bind(char * &&value){
        return sqlite3_bind_text(program,current_id++,std::forward<char *>(value),-1,SQLITE_TRANSIENT);
    }
    inline int Bind(std::string &&value){
        return sqlite3_bind_text(program,current_id++,std::forward<std::string>(value).data(),std::forward<std::string>(value).length(),SQLITE_TRANSIENT);
    }
    inline int Bind(std::string_view value){
        return sqlite3_bind_text(program,current_id++,value.data(),-1,SQLITE_TRANSIENT);
    }

    inline int Bind(wchar_t * &&value){
        return sqlite3_bind_text16(program,current_id++,std::forward<wchar_t *>(value),-1,SQLITE_TRANSIENT);
    }
    inline int Bind(std::wstring &&value){
        return sqlite3_bind_text16(program,current_id++,std::forward<std::wstring>(value).data(),-1,SQLITE_TRANSIENT);
    }


    template<typename T>
    inline void Select(T &value){
        if constexpr(is_instantiation_of_tuple<std::tuple,decltype(Type_to(value))>()){
            auto t=Type_to(value);
            TupleForeach(t,[this](auto &&value){Select(std::forward<decltype(value)>(value));});
            value=std::make_from_tuple<T>(std::move(t));
        }else{
            value=*reinterpret_cast<T*>(sqlite3_column_blob(program,current_id++-1));
        }

    }

    inline void Select(bool &value){
        value=sqlite3_column_int(program,current_id++-1);
    }
    inline void Select(int &value){
        value=sqlite3_column_int(program,current_id++-1);
    }

    inline void Select(sqlite3_int64 &value){
        value=sqlite3_column_int64(program,current_id++-1);
    }
    inline void Select(double &value){
        value=sqlite3_column_double(program,current_id++-1);
    }
    inline void Select(float &value){
        value=sqlite3_column_double(program,current_id++-1);
    }
    inline void Select(sqlite3_value * &value){
        value=sqlite3_column_value(program,current_id++-1);
    }
    inline void Select(const unsigned char * &value){
        value=sqlite3_column_text(program,current_id++-1);
    }
    inline void Select(std::string &value){
        if(auto vl=sqlite3_column_text(program,current_id++-1);vl!=nullptr){
            value=std::string{reinterpret_cast<const char *>(vl)};
        }else{
            value=std::string{};
        }
    }

    void Select_name_collum(std::vector<std::string> &array){
        for(int i=0;i<sqlite3_column_count(program);i++){
            array.push_back(sqlite3_column_name(program,i));
            //const void *sqlite3_column_name16(sqlite3_stmt*, int N);
        }
    };
};

//////////////////////////////////////
    template<int type,int cout_data=0>
    struct Sql_request_info{
        std::string name_table_in="";
        std::string rule="";

        //setting_collums
        std::string name_collums="";
        std::array<int,cout_data> ids=Make_index_to_array<cout_data>();//по умолчанию 0,1,2...cout_data;
        std::vector<std::string> name_collums_array{};

        std::string select_request="";
        std::array<std::string,cout_data> tins;
    };

    template<int cout_data,typename T>
    std::string create_target(std::vector<std::string> &name_collums_array,std::array<T,cout_data> &tins,std::array<int,cout_data> &ids){
       std::string target_request="";
        if(name_collums_array.size()){
            int j=0;
            std::for_each(ids.begin(),ids.end(),[&](auto &&i){target_request+=name_collums_array[i]+tins[j++];});
        }
        return target_request;
    }

    template<int cout_data>
    inline std::string create_sql_request(Sql_request_info<SQLITE_UPDATE,cout_data> &info){
        std::string sql_request="UPDATE ";
        std::string target_request="";

        if(info.name_collums.size()!=0){
            char sep=',';
            for(size_t p=0, q=0; p!=std::string::npos; p=q){
                p+=(p!=0);
                info.name_collums_array.push_back(info.name_collums.substr(p,(q=info.name_collums.find(sep, p))-p));
                q+=(q==0);
            }
        }
        if constexpr(cout_data>0){
            auto tins=Tuple_to_array<std::string>(std::tuple_cat(create_tuple<cout_data-1,std::string>(" =?,"),std::tuple{"=?"}));
            target_request=create_target<cout_data>(info.name_collums_array,tins,info.ids);
        }
        sql_request.append(info.name_table_in+" SET "+target_request+" "+info.rule+" ;");
        return sql_request;
    }

    template<int cout_data>
    inline std::string create_sql_request(Sql_request_info<SQLITE_SELECT,cout_data> &info){
        std::string sql_request="SELECT ";
        std::string target_request="";
        if(info.name_collums.size()!=0){
            target_request=info.name_collums;
        }else{
            auto tins=Tuple_to_array<char>(std::tuple_cat(create_tuple<cout_data-1>(','),std::tuple{' '}));
            target_request=create_target<cout_data>(info.name_collums_array,tins,info.ids);
        }

        if(target_request.size()==0){
            target_request.append("*");
        }
        sql_request.append(target_request+" from "+info.name_table_in+" "+info.rule+" ;");
        return sql_request;
    }

    template<int cout_data>
    inline std::string create_sql_request(Sql_request_info<SQLITE_INSERT,cout_data> &info){
        std::string sql_request="INSERT INTO ";
        std::string target_request;

        if(info.name_collums.size()!=0){
            target_request=info.name_collums;
        }else{
            auto tins=Tuple_to_array<char>(std::tuple_cat(create_tuple<cout_data-1>(','),std::tuple{' '}));
            target_request=create_target<cout_data>(info.name_collums_array,tins,info.ids);
        }

        sql_request.append(info.name_table_in);
        if(info.select_request.size()!=0){
            sql_request.append(" "+info.select_request);
        }else{
            if(target_request.size()!=0){
                sql_request.append(" ("+target_request+") ");
            }
            sql_request.append(" VALUES(");
            auto array=Tuple_to_array<char>(std::tuple_cat(create_tuple<cout_data-1>('?',','),std::tuple{'?'}));
            sql_request.append(array.begin(),array.end());
            sql_request.append(");");
        }
        return sql_request;
    }

    template<int cout_data>
    inline std::string create_sql_request(Sql_request_info<SQLITE_CREATE_TABLE,cout_data> &info){
        std::string sql_request="CREATE TABLE ";
        std::string target_request="";
        std::string cover="\"";
        std::for_each(info.name_collums_array.begin(),info.name_collums_array.end(),[&cover](auto &value){value=cover+value+cover;});

        std::for_each(info.tins.begin(),info.tins.end()-1,[&](auto &value){value+=',';});
        target_request=create_target<cout_data>(info.name_collums_array,info.tins,info.ids);

        sql_request.append(info.name_table_in+" ("+target_request+info.rule+"); ");
        return sql_request;
    }

////////////////////////

template<uint encooding=SQLITE_UTF8,uint flag=SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE>
class Data_base_management{
    std::map<std::string,Stmt> Stmt_buffer;

    Stmt & add_stmt(sqlite3 * db,std::string &&sql_request,unsigned int flag_=0){
        if(Stmt_buffer.find(sql_request)==Stmt_buffer.end()){
            Stmt_buffer.try_emplace(sql_request,db,sql_request,flag_);
        }
        #ifdef tutl_LOG_REQUEST_TO_CONSOLE
             std::cout<<"|"<<sql_request<<std::endl;
        #endif
        return Stmt_buffer[sql_request];
    }

public:
    sqlite3 * db;//Пока нет оболочки над другими функциями это приемлеммо

    Data_base_management(std::string_view patch_db){
        if constexpr(encooding==SQLITE_UTF8){
            sqlite3_open_v2(patch_db.data(), &db,flag,nullptr);
        }else{
            static_assert (true,"encoding is not UTF8");
        }
    }
    Data_base_management(std::basic_string_view<wchar_t> patch_db){
        if constexpr((encooding!=SQLITE_UTF8) && (flag!=(SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE))){
            static_assert (true,"the current version of sqlite does not allow using flags in sqlite3_open16. If you need to use flags, translate the encoding of the db path to UTF-8.");
        }
        if constexpr((encooding==SQLITE_UTF16)||(encooding==SQLITE_UTF16BE)||(encooding==SQLITE_UTF16LE)){
            sqlite3_open16(patch_db.data(), &db);
        }else{
            static_assert (true,"encoding is not UTF16");
        }
    }

    ~Data_base_management(){
        if constexpr(encooding==SQLITE_UTF8){
            sqlite3_close(db);//sqlite3_close_v2
        }
        if constexpr((encooding==SQLITE_UTF16)||(encooding==SQLITE_UTF16BE)||(encooding==SQLITE_UTF16LE)){
            sqlite3_close(db);
        }
    }

    inline void stmt_request(std::string &&request){
        Stmt &stmt=add_stmt(db,std::forward<std::string>(request));
        stmt.use();
        stmt.reset();
    }

    template<typename ...Arg>
    inline void stmt_request_bind(std::string &&request,std::tuple<Arg...> &&data){
        Stmt &stmt=add_stmt(db,std::forward<std::string>(request));
        TupleForeach(std::forward<decltype(data)>(data),[&stmt](auto &&value){stmt.Bind(Type_to(std::forward<decltype(value)>(value)));});
        stmt.use();
        stmt.reset();
    }
    template<typename ...Arg>
    inline void stmt_request_select(std::string &&request,std::vector<std::tuple<Arg...>> &data){
        Stmt &stmt=add_stmt(db,std::forward<std::string>(request));
        while (stmt.use()==SQLITE_ROW) {
            std::tuple<Arg...> t;
            TupleForeach(t,[&stmt](auto &&value){stmt.Select(std::forward<decltype(value)>(value));});
            data.emplace_back(t);
        };
        stmt.reset();
    };


    struct Column_metadata{
        std::string CollName;
        std::string DataType;
        std::string CollSeq;

        bool NotNull;
        bool PrimaryKey;
        bool Autoinc;
    };

    inline std::vector<std::string> get_name_collums(std::string name_table){
        std::vector<std::string> name_collums;
        Stmt &stmt_select=add_stmt(db,"SELECT * from "+name_table+" ;");
        stmt_select.use();
        stmt_select.Select_name_collum(name_collums);
        stmt_select.reset();
        return name_collums;
    }

    inline std::vector<std::string> get_name_tables(){
        std::vector<std::string> name_collums;
        std::vector<std::tuple<std::string>> data;
        stmt_request_select("SELECT name from sqlite_master",data);
        for(auto&T:data){
            name_collums.push_back(std::make_from_tuple<std::string>(std::move(T)));
        }
        return name_collums;
    }

    void create_table(std::string name_table,std::vector<Column_metadata> metadata_collum){
        if(name_table!=""){
            Sql_request_info<SQLITE_CREATE_TABLE> info;
            info.name_table_in=name_table;

            std::vector<std::string> name_collum;
            for(auto &T:metadata_collum){
                name_collum.push_back(T.CollName+(T.NotNull ?" NOT NULL ":" "));
            }
            info.name_collums_array=name_collum;
            int prim=0;
            int autoinc=0;
            for(auto &T:metadata_collum){
                prim+=T.PrimaryKey+T.Autoinc;
                autoinc+=T.Autoinc;
            }
            assert(autoinc>1);
            assert(autoinc==1 && prim!=2);
            if(prim>0){
                info.rule=",PrimaryKey(";
                for(auto &T:metadata_collum){
                    if(T.Autoinc){
                      info.rule+=T.CollName+" AUTOINCREMENT,";
                      break;
                    }else{
                       info.rule+=(T.PrimaryKey ?T.CollName+",":" ");
                    }
                }
                info.rule.erase(info.rule.size()-1,info.rule.size());
                info.rule+=")";
            }

            stmt_request(std::move(create_sql_request(info)));
        }
    };

    template<typename ...Arg>
    void create_table(std::string name_table,std::vector<std::string> name_collum){
        if(name_table!=""){
            constexpr auto c=cout_arg<Arg...>();
            constexpr std::array<std::string_view,5> type_collum={
                "",
                "INTEGER",
                "REAL",
                "TEXT",
                "BLOB"
            };
            std::array<uint,c> types;
            TupleForeachDefining<0,c,uint>(type_disclosure<Arg...>(),types);

            Sql_request_info<SQLITE_CREATE_TABLE,c> info;
            info.name_table_in=name_table;
            for(int i=0;i<c;i++){
               info.tins[i]=" ";
               info.tins[i]+=type_collum[types[i]];
            }
            info.name_collums_array=name_collum;

            stmt_request(std::move(create_sql_request(info)));
        }
    };
    template<typename ...Arg>
    void create_table(std::string &name_table,std::vector<std::string> &name_collum, std::tuple<Arg...> &t){
        create_table<Arg...>(name_table,name_collum);
    }

    template<typename T,typename ...Arg>
    inline static constexpr bool is_type_allowed(){
        return std::is_same<std::array<int,cout_arg<Arg...>()>,T>()       ||
               std::is_same<std::vector<std::string>,T>()                 ||
               std::is_same<std::decay<std::string_view>,std::decay<T>>() ||
               std::is_same<std::decay<const char *>,std::decay<T>>();
    }

    template<typename ...Arg,typename T=std::array<int,cout_arg<Arg...>()>,typename=std::enable_if_t<is_type_allowed<T,Arg...>()>()>
    void insert_values(std::string name_table,std::tuple<Arg...> & data,T values){
        if (name_table!=""){
            Sql_request_info<SQLITE_INSERT,cout_arg<Arg...>()> info;
            info.name_table_in=name_table;

            if constexpr(std::is_same<std::array<int,cout_arg<Arg...>()>,T>()){
                info.name_collums_array=get_name_collums(name_table);
                info.ids=values;
            }
            if constexpr(std::is_same<std::vector<std::string>,T>()){
                info.name_collums_array=values;
            }
            if constexpr(std::is_same<std::decay<std::string_view>,std::decay<T>>()||std::is_same<std::decay<const char *>,std::decay<T>>()){
                info.name_collums=values;
            }

            stmt_request_bind(std::move(create_sql_request(info)),type_disclosure(data));
        }
    };

    template<typename ...Arg>
    void insert_values(std::string name_table,std::tuple<Arg...> & data){
            insert_values<Arg...>(name_table,data,"");
    };

    struct Base_tuple{};

    template<typename ...Arg>
    struct Tuple_R:public Base_tuple{
        Tuple_R(){}
        Tuple_R(std::tuple<Arg...> &&data):data_(std::forward<decltype (data)>(data)){}

        std::tuple<Arg...> data_;
    };

    template<int N,int K,typename T=std::array<int,N>,typename=std::enable_if_t<is_type_allowed<T,N>()>(),
                         typename J=std::array<int,K>,typename=std::enable_if_t<is_type_allowed<J,K>()>()>
    void insert_select(std::string name_table_into,T values_into,std::string name_table_from,J values_from,std::string rule=""){
        if (name_table_into!=""){

            Sql_request_info<SQLITE_INSERT,N> info_insert;
            info_insert.name_table_in=name_table_into;

            if constexpr(std::is_same<std::array<int,N>,T>()){
                info_insert.name_collums_array=get_name_collums(name_table_into);
                info_insert.ids=values_into;
            }
            if constexpr(std::is_same<std::vector<std::string>,T>()){
                info_insert.name_collums_array=values_into;
            }
            if constexpr(std::is_same<std::decay<std::string_view>,std::decay<T>>()||std::is_same<std::decay<const char *>,std::decay<T>>()){
                info_insert.name_collums=values_into;
            }


            Sql_request_info<SQLITE_SELECT,K> info_select;
            info_select.name_table_in=name_table_from;
            info_select.rule=rule;

            if constexpr(std::is_same<std::array<int,K>,J>()){
                info_select.name_collums_array=get_name_collums(name_table_from);
                info_select.ids=values_from;
            }
            if constexpr(std::is_same<std::vector<std::string>,J>()){
                info_select.name_collums_array=values_from;
            }
            if constexpr(std::is_same<std::decay<std::string_view>,std::decay<J>>()||std::is_same<std::decay<const char *>,std::decay<J>>()){
                info_select.name_collums=values_from;
            }

            info_insert.select_request=create_sql_request(info_select);
            stmt_request(std::move(create_sql_request(info_insert)));
        }
    }
    void insert_select(std::string name_table_into,std::string name_table_from,std::string rule=""){
        insert_select<1,1>(name_table_into,"",name_table_from,"",rule);
    }
    template<int N,typename T=std::array<int,N>>
    void insert_select(std::string name_table_into,T values_into,std::string name_table_from,std::string rule=""){
        insert_select<N,1>(name_table_into,values_into,name_table_from,"",rule);
    }
    template<int K,typename J=std::array<int,K>>
    void insert_select(std::string name_table_into,std::string name_table_from,J values_from,std::string rule=""){
        insert_select<1,K>(name_table_into,"",name_table_from,values_from,rule);
    }

    void insert_defolt(std::string name_table){
        if (name_table!=""){
            std::string sql_request="INSERT INTO "+name_table+" DEFAULT VALUES";
            stmt_request(sql_request);
            std::cout<<db<<sql_request<<std::endl;
        }
    }


    template<typename ...Arg,typename T=std::array<int,cout_arg<Arg...>()>,typename=std::enable_if_t<is_type_allowed<T,Arg...>()>()>
    void update_values(std::string name_table,std::tuple<Arg...> & data,T values,std::string_view rule=""){
        if (name_table!=""){
            Sql_request_info<SQLITE_UPDATE,cout_arg<Arg...>()> info;
            info.name_table_in=name_table;
            info.rule=rule;

            if constexpr(std::is_same<std::array<int,cout_arg<Arg...>()>,T>()){
                info.name_collums_array=get_name_collums(name_table);
                info.ids=values;
            }
            if constexpr(std::is_same<std::vector<std::string>,T>()){
                info.name_collums_array=values;
            }
            if constexpr(std::is_same<std::decay<std::string_view>,std::decay<T>>()||std::is_same<std::decay<const char *>,std::decay<T>>()){
                info.name_collums=values;
            }
            stmt_request_bind(std::move(create_sql_request(info)),type_disclosure(data));

        }
    };
    template<typename ...Arg>
    void update_values(std::string name_table,std::tuple<Arg...> & data,std::string_view rule=""){
        update_values<Arg...>(name_table,data,get_name_collums(name_table),rule);
    };


//    void update_select(std::string name_table_into,std::string name_table_from,std::string rule1="*",std::string rule2=""){
//        if (name_table_into!=""){
//            std::string sql_request="INSERT INTO "+name_table_into+" SELECT "+rule1+" from "+name_table_from+" "+rule2;
//            Stmt stmt(db,sql_request);
//            stmt.use();
//            std::cout<<db<<sql_request<<std::endl;
//        }
//    };



    template<typename ...Arg,typename T=std::array<int,cout_arg<Arg...>()>,typename=std::enable_if_t<is_type_allowed<T,Arg...>()>()>
    std::vector<std::tuple<Arg...>> select(std::string name_table,T values,std::string_view rule=""){
        std::vector<std::tuple<Arg...>> data;
        if (name_table!=""){
            Sql_request_info<SQLITE_SELECT,cout_arg<Arg...>()> info;
            info.name_table_in=name_table;
            info.rule=rule;
            if constexpr(std::is_same<std::array<int,cout_arg<Arg...>()>,T>()){
                info.name_collums_array=get_name_collums(name_table);
                info.ids=values;
            }
            if constexpr(std::is_same<std::vector<std::string>,T>()){
                info.name_collums_array=values;
            }
            if constexpr(std::is_same<std::decay<std::string_view>,std::decay<T>>()||std::is_same<std::decay<const char *>,std::decay<T>>()){
                info.name_collums=values;
            }

            stmt_request_select(std::move(create_sql_request(info)),data);
        }
        return data;
    }

    template<typename ...Arg>
    std::vector<std::tuple<Arg...>> select(std::string name_table,std::string_view rule=""){
        return select<Arg...>(name_table,"",rule);
    }

    template<typename ...Arg,typename T=std::array<int,cout_arg<Arg...>()>,typename=std::enable_if_t<is_type_allowed<T,Arg...>()>()>
    std::vector<std::tuple<Arg...>> select(std::string name_table,T values,std::tuple<Arg...> t,std::string_view rule=""){
        return select<Arg...>(name_table,values,rule);
    }


    template<typename ...Arg>
    std::vector<std::tuple<Arg...>> select(std::string name_table,std::tuple<Arg...>,std::string_view rule=""){
        return select<Arg...>(name_table,"",rule);
    }

    template<typename ...Arg>
    void select(std::string name_table,std::vector<std::tuple<Arg...>> &data,std::string_view rule=""){
        data=select<Arg...>(name_table,"",rule);
    }

    void delete_table(std::string name_table,std::string rule=""){
        if (name_table!=""){
            stmt_request("DROP TABLE "+name_table+" "+rule);
        }
    }

    void delete_rows(std::string name_table,std::string rule){
        if (name_table!=""){
            stmt_request("DELETE FROM "+name_table+" "+rule);
        }
    }

};

};
#endif // DOWNLOAD_MANAGEMENT_H
