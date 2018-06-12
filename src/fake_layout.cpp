#include <iostream>
#include <sstream>
#include <fstream>
#include <climits>
#include <iostream>
#include <iomanip> 
#include <vector>
#include <string>
#include <unordered_set>
#include "layout.h"
#include "util.h"
using namespace std;
//#define DEBUG
unsigned Polygon::global_ref=0;

void Layer::initRule(int n1, int n2, int n3, double min, double max)
{
    min_width = n1;
    min_space = n2;
    max_fill_width = n3;
    min_density = min;
    max_density = max;
}
void print_Polygon(Polygon* T){
    cerr<<T->getType()<<" ("<<T->_top_right_x()<<","<<T->_top_right_y()<<") ("<<T->_bottom_left_x()<<","<<T->_bottom_left_y()<<")\n";
    int a;
    //if(T->_top_right_x()==3407008&&T->_bottom_left_x()==3407008)cin>>a;
    if(T->getType()[0]=='d')return;
    cerr<<"tr "<<T->get_tr()->getType()<<" ("<<T->get_tr()->_top_right_x()<<","<<T->get_tr()->_top_right_y()<<") ("<<T->get_tr()->_bottom_left_x()<<","<<T->get_tr()->_bottom_left_y()<<")\n";
    //if(T->get_tr()->_top_right_x()==3407008&&T->get_tr()->_bottom_left_x()==3407008)cin>>a;
    cerr<<"rt "<<T->get_rt()->getType()<<" ("<<T->get_rt()->_top_right_x()<<","<<T->get_rt()->_top_right_y()<<") ("<<T->get_rt()->_bottom_left_x()<<","<<T->get_rt()->_bottom_left_y()<<")\n";
    //if(T->get_rt()->_top_right_x()==3407008&&T->get_rt()->_bottom_left_x()==3407008)cin>>a;
    cerr<<"lb "<<T->get_lb()->getType()<<" ("<<T->get_lb()->_top_right_x()<<","<<T->get_lb()->_top_right_y()<<") ("<<T->get_lb()->_bottom_left_x()<<","<<T->get_lb()->_bottom_left_y()<<")\n";
    //if(T->get_lb()->_top_right_x()==3407008&&T->get_lb()->_bottom_left_x()==3407008)cin>>a;
    cerr<<"bl "<<T->get_bl()->getType()<<" ("<<T->get_bl()->_top_right_x()<<","<<T->get_bl()->_top_right_y()<<") ("<<T->get_bl()->_bottom_left_x()<<","<<T->get_bl()->_bottom_left_y()<<")\n";
    //if(T->get_bl()->_top_right_x()==3407008&&T->get_bl()->_bottom_left_x()==3407008)cin>>a;

}
void Layer::initialize_layer(int x_bl, int y_bl, int x_tr, int y_tr){
    #ifdef DEBUG
        cout<<"init layer "<<endl;
    #endif
    _tr_boundary_x = x_tr;
    _tr_boundary_y = y_tr;
    _bl_boundary_x = x_bl;
    _bl_boundary_y = y_bl;
    //一開始空的大space
    Polygon *a=new Polygon("space");
    a->set_xy(_tr_boundary_x,_tr_boundary_y,_bl_boundary_x,_bl_boundary_y);
    int x1=_tr_boundary_x;
    int y1=_tr_boundary_y;
    int x2=_bl_boundary_x;
    int y2=_bl_boundary_y;
    //dummy left
    dummy_left=new Polygon("dummy left");
    dummy_left->set_xy(x2,y1,x2-2,y2);
    a->set_bl(dummy_left);
    //dummy right
    dummy_right=new Polygon("dummy right");
    dummy_right->set_xy(x1+2,y1,x1,y2);
    a->set_tr(dummy_right);
    //dummy top
    dummy_top=new Polygon("dummy top");
    dummy_top->set_xy(x1,y1+2,x2,y1);
    a->set_rt(dummy_top);
    dummy_top->set_lb(a);
    //dummy bottom
    dummy_bottom=new Polygon("dummy bottom");
    dummy_bottom->set_xy(x1,y2,x2,y2-2);
    a->set_lb(dummy_bottom);
    dummy_bottom->set_rt(a);

    dummy_bottom_right = new Polygon("dummy bottom right");
    dummy_bottom_right->set_xy(x1+2, y2, x1, y2-1);
    dummy_bottom_right->set_rt(dummy_right);
    dummy_bottom_right->set_bl(dummy_bottom);
    dummy_right->set_lb(dummy_bottom_right);
    dummy_bottom->set_tr(dummy_bottom_right);

    dummy_right_top = new Polygon("dummy right top");
    dummy_right_top->set_xy(x1+2, y1+2, x1, y1);
    dummy_right_top->set_lb(dummy_right);
    dummy_right_top->set_bl(dummy_top);
    dummy_right->set_rt(dummy_right_top);
    dummy_top->set_tr(dummy_right_top);

    dummy_bottom_left = new Polygon("dummy bottom left");
    dummy_bottom_left->set_xy(x2, y2, x2-2, y2-2);
    dummy_bottom_left->set_rt(dummy_left);
    dummy_bottom_left->set_tr(dummy_bottom);
    dummy_left->set_lb(dummy_bottom_left);
    dummy_bottom->set_bl(dummy_bottom_left);

    dummy_top_left = new Polygon("dummy top right");
    dummy_top_left->set_xy(x2, y1+2, x2-2, y1);
    dummy_top_left->set_lb(dummy_left);
    dummy_top_left->set_tr(dummy_top);
    dummy_left->set_rt(dummy_top_left);
    dummy_top->set_bl(dummy_top_left);
}
Polygon* Layer::point_search(Polygon* start,int x,int y){
    /* 當x,y有切齊的時候 我們會選被包在框框裡面的tile 因此是
    top y = y  or left x = x 的時候
    */
    #ifdef DEBUG
        cout<<"point search "<<endl;
    #endif
    Polygon* current=start;
    //如果要query的座標比現在的x還要大或還要小且 要query的座標比現在的y還要大或還要小
    while((x>=current->_top_right_x()||x<current->_bottom_left_x())||(y>current->_top_right_y()||y<=current->_bottom_left_y())){
        while((y>=current->_top_right_y()||y<=current->_bottom_left_y())){
            if(y==current->_top_right_y())break;
            if(y>current->_top_right_y())current=current->get_rt();
            else current=current->get_lb();
        }
        while((x>=current->_top_right_x()||x<=current->_bottom_left_x())){
            if(x==current->_bottom_left_x())break;
            if(x>=current->_top_right_x())current=current->get_tr();
            else current=current->get_bl();
        }
        //current== 2200000 2477214
    }
    return current;
}
void neighbor_find_own(Polygon* T,vector<Polygon*> &v){
    //上到下找
    #ifdef DEBUG
        cout<<"find own "<<endl;
    #endif
    Polygon* current=T->get_tr();
    while(current->_bottom_left_y()>=T->_bottom_left_y()){
        v.push_back(current);
        current=current->get_lb();
    }
}
void enumerate(Polygon* T,vector<Polygon*> &v,int max_x){
    //找own
    #ifdef DEBUG
        cout<<"enumerate "<<endl;
    #endif
    if(T->isglobalref())return;
    v.push_back(T);
    T->setToglobalref();
    //not sure top_right_x or top_right_y
    if(T->_top_right_x()>=max_x)
        return;
    vector<Polygon*>neighbor;
    neighbor_find_own(T,neighbor);
    for(int i=0;i<neighbor.size();i++){
        enumerate(neighbor[i],v,max_x);
    }
    return;
}


vector<Polygon*> Layer::region_query(Polygon* start,int x1,int y1,int x2,int y2){
    /*x1,y1 是右上   x2,y2 是左下
    左上角的座標是 x2,y1 是我們要query的
    */
    #ifdef DEBUG
        cout<<"region query "<<endl;
    #endif
    Polygon::setGlobalref();
    vector<Polygon*> query_Polygon;
    vector<Polygon*>left_poly;
    start=point_search(start,x2,y1);
    while(start->_top_right_y()>y2){
        left_poly.push_back(start);
        start=point_search(start,x2,start->_bottom_left_y());
    }
    //left_poly.push_back(start);//最後那塊跳出來的時候 也算在裡面 參照region query 8號tile
    for(int i=0;i<left_poly.size();i++){
        enumerate(left_poly[i],query_Polygon,x1);
    }
    #ifdef DEBUG
        cout<<"query"<<query_Polygon.size()<<endl;
        cout<<"le"<<left_poly.size()<<endl;
    #endif
    //當region query最下面那排要特別處理
    //找最下面那排有哪些tile
    vector<Polygon*>bottom_poly;
    Polygon* T=left_poly[left_poly.size()-1];
    //Polygon* T=left_poly[0];
    while(T->_bottom_left_x()<x1){
        bottom_poly.push_back(T);
        T=point_search(T,T->_top_right_x(),y2+1);
    }
    #ifdef DEBUG
        cout<<"bot"<<bottom_poly.size()<<endl;
    #endif
    Polygon* aaa=left_poly[0];
    Polygon* bbb=bottom_poly[0];
    for(int i=0;i<bottom_poly.size();i++){
        enumerate(bottom_poly[i],query_Polygon,x1);
    }
    #ifdef DEBUG
        cout<<"query2"<<query_Polygon.size()<<endl;
    #endif
    return query_Polygon;
}
vector<Polygon*> Layer::region_query(Polygon* start,Polygon* T){
    return region_query(start,T->_top_right_x(),T->_top_right_y(),T->_bottom_left_x(),T->_bottom_left_y());
}
Polygon* Layer::split_Y(Polygon* &bigGG,int y,bool is_top){
    #ifdef DEBUG
        cout<<"split y "<<endl;
    #endif     
    Polygon* new_poly=new Polygon(bigGG->getType(),bigGG->is_solid());
    if(is_top){
        new_poly->set_xy(bigGG->_top_right_x(),bigGG->_top_right_y(),bigGG->_bottom_left_x(),y);
        new_poly->set_rt(bigGG->get_rt());
        new_poly->set_tr(bigGG->get_tr());
        new_poly->set_lb(bigGG);
        new_poly->set_bl(bigGG->get_bl());
        //point_search(bigGG,bigGG->_bottom_left_x()-1,y)
        //把上面的東西指回來新的
        Polygon *tp;
        for(tp=bigGG->get_rt();tp->get_lb()==bigGG;tp=tp->get_bl())
            tp->set_lb(new_poly);
        bigGG->set_rt(new_poly);
        //inserted->set_rt(new_poly);
        //把左邊的人的tr指回來
        for(tp=bigGG->get_bl();tp->_top_right_y()<=bigGG->_top_right_y();tp=tp->get_rt()){
            if(tp->_top_right_y()>y)
                tp->set_tr(new_poly);
        }
        //把右邊的bl指回來
        for(tp=bigGG->get_tr();tp->_bottom_left_y()>=y;tp=tp->get_lb())
            tp->set_bl(new_poly);
        bigGG->set_tr(tp);
        
        //bigGG->set_tr(point_search(new_poly,bigGG->_top_right_x()+1,y));
        bigGG->set_xy(bigGG->_top_right_x(), y ,bigGG->_bottom_left_x(),bigGG->_bottom_left_y());
    }
    else{   
        new_poly->set_xy(bigGG->_top_right_x(),y,bigGG->_bottom_left_x(),bigGG->_bottom_left_y());
        new_poly->set_rt(bigGG);
        new_poly->set_lb(bigGG->get_lb());
        new_poly->set_bl(bigGG->get_bl());
        new_poly->set_tr(bigGG->get_tr());//point_search(bigGG,bigGG->_top_right_x()+1,y)
        //把下面的指回來上面
        Polygon *tp;
        for(tp=bigGG->get_lb();tp->get_rt()==bigGG;tp=tp->get_tr())
            tp->set_rt(new_poly);
        //把右邊的指回來
        for(tp=bigGG->get_tr();tp->_bottom_left_y()>=y;tp=tp->get_lb())
            ;
        new_poly->set_tr(tp);
        for(;tp->get_bl()==bigGG;tp=tp->get_lb())
            tp->set_bl(new_poly);
        //左邊的指回來
        for(tp=bigGG->get_bl();tp->_top_right_y()<=y;tp=tp->get_rt())
            tp->set_tr(new_poly);
        //set bigGG
        bigGG->set_bl(tp);
        bigGG->set_xy(bigGG->_top_right_x(),bigGG->_top_right_y(),bigGG->_bottom_left_x(),y);
        bigGG->set_lb(new_poly);
        //inserted->set_lb(new_poly);
        //bigGG->set_bl(point_search(bigGG,bigGG->_bottom_left_x()-1,y));
    }
    return new_poly;

}
Polygon* Layer::split_X_left(Polygon* &bigGG, int x_left,int x_right){
    #ifdef DEBUG
        cout<<"split x_left "<<endl;
    #endif
    Polygon* tp;
    Polygon* new_poly = new Polygon(bigGG->getType(), bigGG->is_solid());
    new_poly->set_xy(x_left,bigGG->_top_right_y(),bigGG->_bottom_left_x(),bigGG->_bottom_left_y());
    new_poly->set_tr(bigGG);
    new_poly->set_lb(bigGG->get_lb());
    new_poly->set_bl(bigGG->get_bl());
    bigGG->set_xy(bigGG->_top_right_x(),bigGG->_top_right_y(),x_left,bigGG->_bottom_left_y());
    //把左邊的指回來 
    for(tp=new_poly->get_bl();tp->get_tr()==bigGG;tp=tp->get_rt())
        tp->set_tr(new_poly);
    //把上面的接回來
    for(tp=bigGG->get_rt();tp->_bottom_left_x()>=x_left;tp=tp->get_bl())
        ;
    new_poly->set_rt(tp);
    for(;tp->get_lb()==bigGG;tp=tp->get_bl())
        tp->set_lb(new_poly);
    //下面指回來
    for(tp=bigGG->get_lb();tp->_top_right_x()<=x_left;tp=tp->get_tr())
        tp->set_rt(new_poly);
    bigGG->set_lb(tp);
    bigGG->set_bl(new_poly);
    return new_poly;
}
Polygon* Layer::split_X_right(Polygon* &bigGG, int x_left,int x_right ){
    #ifdef DEBUG
        cout<<"split x_right "<<endl;
    #endif
    Polygon* tp;
    Polygon* new_poly = new Polygon(bigGG->getType(), bigGG->is_solid());
    new_poly->set_xy(bigGG->_top_right_x(),bigGG->_top_right_y(),x_right,bigGG->_bottom_left_y());
    new_poly->set_rt(bigGG->get_rt());
    new_poly->set_tr(bigGG->get_tr());
    new_poly->set_bl(bigGG);
    bigGG->set_xy(x_right,bigGG->_top_right_y(),bigGG->_bottom_left_x(),bigGG->_bottom_left_y());
    //上面接回來
    for(tp=bigGG->get_rt();tp->_bottom_left_x()>=x_right;tp=tp->get_bl())
        tp->set_lb(new_poly);
    bigGG->set_rt(tp);
    // 把下面指回來
    for(tp=bigGG->get_lb();tp->_top_right_x()<=x_right;tp=tp->get_tr())
        ;
    new_poly->set_lb(tp);
    while(tp->get_rt()==bigGG){
        tp->set_rt(new_poly);
        tp=tp->get_tr();
    }
    //右邊指回來
    for(tp=bigGG->get_tr();tp->get_bl()==bigGG;tp=tp->get_lb())
        tp->set_bl(new_poly);
    bigGG->set_tr(new_poly);
    return new_poly;
}
void join(Polygon* T1,Polygon *T2){
    Polygon* tp;
    if(T1->_bottom_left_x()==T2->_bottom_left_x()&&T1->_top_right_x()==T2->_top_right_x()&&T1->_bottom_left_y()==T2->_top_right_y()){
        T1->set_xy(T1->_top_right_x(),T1->_top_right_y(),T2->_bottom_left_x(),T2->_bottom_left_y());
        //沿著右邊指回來
        #ifdef DEBUG
        cout<<"joining"<<endl;
        #endif
        for(tp=T2->get_tr();tp->get_bl()==T2;tp=tp->get_lb()){
            tp->set_bl(T1);
        }
        for(tp=T2->get_bl();tp->get_tr()==T2;tp=tp->get_rt()){
            tp->set_tr(T1);
        }
        //把下面網上指導t2的指導t1
        for(tp=T2->get_lb();tp->get_rt()==T2;tp=tp->get_tr()){
            tp->set_rt(T1);
        }
        T1->set_lb(T2->get_lb());
        T1->set_bl(T2->get_bl());
        delete T2;
        T2=NULL;
        return;
    }
    else return;
}

bool Layer::insert(vector<int> tokens,bool is_cnet){
    // query 是給 先右上 再左下
    //因為soild 會有最小間距 所以我們query看能不能夠塞的時候 
    //要看大塊一點 四周都加最小間距
    _b_left_x = ;
        _b_left_y = tokens[2];
       
        _t_right_y = 
    #ifdef DEBUG
        cout<<"insert "<<tokens[3]<<" "<<tokens[4]<<endl;
    #endif
    int bl_x,bl_y,tr_x,tr_y;
    //決定query的邊界
    //左下角
    (tokens[1]-get_gap()>=get_bl_boundary_x()) ? bl_x = tokens[1] - get_gap() : bl_x = get_bl_boundary_x();
    (tokens[2]-get_gap()>=get_bl_boundary_y()) ? bl_y = tokens[2] - get_gap() : bl_y = get_bl_boundary_y();
    //右上角
    (tokens[3] + get_gap() >get_tr_boundary_x() ) ? tr_x = get_tr_boundary_x() : tr_x = tokens[3] + get_gap();
    (tokens[4] + get_gap() >get_tr_boundary_y() ) ? tr_y = get_tr_boundary_y() : tr_y = tokens[4] + get_gap();
    
    // 如果要插進去的這塊裡面有solid就不能插 就像有男友的女生一樣
    //vector<Polygon*> query_list=region_query(dummy_bottom,tr_x,tr_y,bl_x,bl_y);// (start,跟要插進去得tile)
    vector<Polygon*> query_list=region_query(dummy_bottom,tokens[3],tokens[4],tokens[1],tokens[2]);
    for(int i=0;i<query_list.size();i++){
        if(query_list[i]->is_solid())
            return false;
    }
    Polygon*aa;
    bool is_left= ( tokens[1] != get_bl_boundary_x() );
    bool is_right= (tokens[3] != get_tr_boundary_x() );
    #ifdef DEBUG
        cout<<"query_list num= "<<query_list.size()<<endl;
    #endif
    /*

    split y

    */
    //cout<<"q_list size == "<<query_list.size()<<endl;
    for(int i=0; i<query_list.size(); i++){
        /*
        if(query_list[i]->_top_right_y() == T->_top_right_y()){
            //T->set_rt(point_search(query_list[i],T->_top_right_x(),T->_top_right_y()+1));
            if(T->_top_right_y()==get_tr_boundary_y()&&T->_bottom_left_x()==get_bl_boundary_x())
                //dummy_top->set_lb(T);
        }
        if(query_list[i]->_bottom_left_y() == T->_bottom_left_y()){
            //T->set_lb(point_search(query_list[i],T->_bottom_left_x(),T->_bottom_left_y()-1));
            if(T->_bottom_left_y()==get_bl_boundary_y()&&T->_top_right_x()==get_tr_boundary_x())
                //dummy_bottom->set_rt(T);
        }
        */
        if(query_list[i]->_top_right_y() > tokens[4])
            split_Y(query_list[i], tokens[4], true);
        if(query_list[i]->_bottom_left_y() < tokens[2])
            split_Y(query_list[i],tokens[2],false);
    }
    //cout<<"a y1 = "<< aa->_bottom_left_y()<<endl;
    vector<Polygon*> split_x_right;
    vector<Polygon*> split_x_left;
    //vector<Polygon*> bigGG_list;
    /*

    split x

    */
    #ifdef DEBUG
        cout<<"query_list num= "<<query_list.size()<<endl;
    #endif
    for(int i=0;i<query_list.size();i++){

        if(is_left){
            if(query_list[i]->_bottom_left_x() < tokens[1]){
                Polygon* a = split_X_left(query_list[i],tokens[1],tokens[3]);
            //if(a->_bottom_left_y()<=T->_bottom_left_y())
            //    T->set_bl(a);
                split_x_left.push_back(a);
            }
        }
        /*
        else {
            T->set_bl(dummy_left);
            if(T->_top_right_y()==get_tr_boundary_y())dummy_left->set_tr(T);

        }*/
        if(is_right){
            if(query_list[i]->_top_right_y() > tokens[4]){
                Polygon* a = split_X_right(query_list[i],tokens[1],tokens[3]);
            //if(a->_top_right_y()>=T->_top_right_y())
            //    T->set_tr(a);
                split_x_right.push_back(a);
            }
        }
        /*
        else  {
            T->set_tr(dummy_right);
            if(T->_bottom_left_y()==get_bl_boundary_y())dummy_right->set_bl(T);
        }*/
    }
    /*
    after splitting we join 

    */
    for(int i=query_list.size()-1;i>=1;i--)
        join(query_list[i-1],query_list[i]);
    /*
    for( std::vector<Polygon*>::iterator i = query_list.begin()+1, endI = query_list.end(); i != endI; ++i)
        delete *i;
    */
    
    for(int i=split_x_left.size()-1;i>=1;i--)
        join(split_x_left[i-1],split_x_left[i]);
    for(int i=split_x_right.size()-1;i>=1;i--)
        join(split_x_right[i-1],split_x_right[i]);
    #ifdef DEBUG
        cout<<"query_list num= "<<query_list.size()<<endl;
        print_Polygon(query_list[0]);
        //if(query_list.size()>1)print_Polygon(query_list[1]);
    #endif
    /*
    for( std::vector<Polygon*>::iterator i = query_list.begin()+1, endI = query_list.end(); i != endI; ++i)
        delete *i;
    */
    query_list
    query_list[0]->setToSolid();
    query_list[0]->setType(T->getType());
    query_list[0]->set_layer_id(T->get_layer_id());
    query_list[0]->set_net_id(T->get_net_id());
    query_list[0]->set_polygon_id(T-> get_polygon_id());
    return true;

}


