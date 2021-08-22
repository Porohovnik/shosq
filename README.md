# shosq
## Slight shell over sqlite3
Header-only library ,with allocation memory  only for generating an sql query
### Example
```C++
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
        
        std::tuple<int,float,std::string,A> t={555,0.2,"Hello world",{4.0,5.0,6.0}};
        db.insert_values("test",t);
        
        db.create_table<int,float,std::string,A>("test",{"i","f","s","x","y","z"});
        db.create_table<int,float,std::string>("test1",{"i","f","s"});
        
        std::tuple<std::string,float,int> t_1={"Hello world",0.3,555};
        db.insert_values("test1",t_1,{2,1,0});

        std::tuple<std::string,float,int> t_2={"!Hello world",0.3,555};
        db.insert_values("test1",t_2,"s,f,i");

        std::tuple<std::string,float,int> t_3={"Hello world!",0.3,555};
        std::vector<std::string> v={"s","f","i"};
        db.insert_values("test1",t_3,v);

    }
        
```
