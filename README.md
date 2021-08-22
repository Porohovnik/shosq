# shosq
## Slight shell over sqlite3
Header-only library ,with allocation memory  only for generating an sql query
### Example insert
```C++    
    #include <shosq.h>
    #include <string>
    #include <tuple>
    #include <vector>

    struct A{
        float x;
        float y;
        float z;
    }
    //begin rule conversion type
    template<> constexpr auto shosq::Type_to<>(A vec){
        return std::tuple(vec.x,vec.y,vec.z);
    };
    //end rule conversion type 
    
    int main(){
        shosq::Data_base_management db("name_db.db");
       
       
        db.create_table<int,float,std::string>("test",{"i","f","s"});
        db.create_table<int,float,std::string,A>("test1",{"i","f","s","x","y","z"});
        
        //signature insert
        //db.insert(name_table_in,data,column_order_in(optional));
        //column order can be one of three options:
        //std::array<int,N>
        //std::vector<std::string>
        //std::string
       
       
        //normal insertion
        std::tuple<int,float,std::string,A> t={555,0.2,"Hello world",{4.0,5.0,6.0}};
        db.insert_values("test1",t);
        
        //using indexes, we set the order of the inserted elements
        std::tuple<std::string,float,int> t_1={"Hello world",0.3,555};
        db.insert_values("test",t_1,{2,1,0});

        //using column names, we set the order of the inserted elements
        std::tuple<std::string,float,int> t_2={"!Hello world",0.3,555};
        db.insert_values("test",t_2,"s,f,i");

        //using the column names array, we set the order of the inserted elements
        std::tuple<std::string,float,int> t_3={"Hello world!",0.3,555};
        std::vector<std::string> v={"s","f","i"};
        db.insert_values("test",t_3,v);

    }  
```
### Example insert-select
```C++

    
    #include <shosq.h>
    #include <string>
    #include <vector>

    int main(){
        shosq::Data_base_management db("name_db.db");
        
        //signature insert_select
        //db.insert_select(name_table_in,column_order_in(optional),name_table_from,column_order_from(optional),rule_select(optional));
        //column order can be one of three options:
        //std::array<int,N>
        //std::vector<std::string>
        //std::string
        
        //inserting data from "table_" into "table"
        db.insert_select("test_","test");
        
        std::vector<std::string> v={"s","f","i"};
        db.insert_select("test",{2,1,0},"test_",v,"Where i>5");

    }  
```
### Example select
```C++
    #include <shosq.h>
    #include <string>
    #include <vector>
    #include <tuple>
    
    struct A{
        float x;
        float y;
        float z;
    }
    //begin rule conversion type
    template<> constexpr auto shosq::Type_to<>(A vec){
        return std::tuple(vec.x,vec.y,vec.z);
    };
    //end rule conversion type 
    
    int main(){
        shosq::Data_base_management db("name_db.db");
        
        //signature select
        //db.select<type_elemens_select>(name_table_from,column_order_in(optional));
        //type output: std::vector<std::tuple<Arg...>>
        //column order can be one of three options:
        //std::array<int,N>
        //std::vector<std::string>
        //std::string
        
        auto t3=db.select<int,float,std::string,A>("test1");
        for(auto &T:t3){
            std::cout<<std::get<0>(T)<<":"<<std::get<1>(T)<<":"<<std::get<2>(T)<<
            "("<<std::get<3>(T).x<<","<<std::get<3>(T).y<<","<<std::get<3>(T).z<<")"<<std::endl;
        }

        auto t4=db.select<std::string,int,float>("test",{2,0,1});
        for(auto &T:t4){
            std::cout<<std::get<0>(T)<<":"<<std::get<1>(T)<<":"<<std::get<2>(T)<<std::endl;
        }
    }  
```

### Example update
```C++
    #include <shosq.h>
    #include <string>
    #include <vector>
    #include <tuple>
    
    struct A{
        float x;
        float y;
        float z;
    }
    //begin rule conversion type
    template<> constexpr auto shosq::Type_to<>(A vec){
        return std::tuple(vec.x,vec.y,vec.z);
    };
    //end rule conversion type 
    
    int main(){
        shosq::Data_base_management db("name_db.db");
        
        //signature update
        //db.insert(name_table_in,data,column_order_in(optional),rule(optional));
        //column order can be one of three options:
        //std::array<int,N>
        //std::vector<std::string>
        //std::string
        
        std::tuple<int,float,std::string,A> t={555,0.2,"Hello world",{4.0,5.0,6.0}};
        db.update_values("test1",t,"Field1,Field2,Field3,Field4,Field5,Field6");
        
        db.update_values("test1",t,"Where i>0")
        db.update_values("test1",t,"i,f,s,x,y,z","Where i>0");
        db.update_values("test1",t,{0,1,2,3,4,5});
        
        std::vector<std::string> v={"i","f","s","x","y","z"};
        db.update_values("test1",t,v);
    }  
