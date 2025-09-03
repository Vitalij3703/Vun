#include <iostream>
#include <string>
class n {
    auto value;
    n(auto val){
        this.value = val;
    }
}
class num_n {
    public:
        int value;
        num_n(int value){
            this.value = value;
        }
}
class str_n {
    public:
        string value;
        str_n(string value){
            this.value = value;
        }
}
class var_n {
    public:
        string id;
        num_n numval;
        str_n strval;
        var_n(string id, int val){
            this.id = id;
            this.numval = val;
        }
        var_n(string id, string val){
            this.id = id;
            this.numval = val;
        }
}