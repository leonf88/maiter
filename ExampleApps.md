# PageRank #

```
#include "client/client.h"


using namespace dsm;
DECLARE_string(result_dir);
DECLARE_int64(num_nodes);
DECLARE_double(portion);

struct PagerankIterateKernel : public IterateKernel<int, float, vector<int> > {
    float zero;

    PagerankIterateKernel() : zero(0){}

    void read_data(string& line, int& k, vector<int>& data){
        string linestr(line);
        int pos = linestr.find("\t");
        if(pos == -1) return;
        
        int source = boost::lexical_cast<int>(linestr.substr(0, pos));

        vector<int> linkvec;
        string links = linestr.substr(pos+1);
        if(*links.end()!=' '){
            links=links+" ";
        }
        int spacepos = 0;
        while((spacepos = links.find_first_of(" ")) != links.npos){
            int to;
            if(spacepos > 0){
                to = boost::lexical_cast<int>(links.substr(0, spacepos));
            }
            links = links.substr(spacepos+1);
            linkvec.push_back(to);
        }

        k = source;
        data = linkvec;
    }

    void init_c(const int& k, float& delta,vector<int>& data){
        delta = 0.2;
    }

    void init_v(const int& k,float& v,vector<int>& data){
        v=0;  
    }
    
    void accumulate(float& a, const float& b){
            a = a + b;
    }

    void priority(float& pri, const float& value, const float& delta){
            pri = delta;
    }

    void g_func(const int& k,const float& delta, const float&value, const vector<int>& data, vector<pair<int, float> >* output){
            int size = (int) data.size();
            float outv = delta * 0.8 / size;
            for(vector<int>::const_iterator it=data.begin(); it!=data.end(); it++){
                    int target = *it;
                    output->push_back(make_pair(target, outv));
            }
    }

    const float& default_v() const {
            return zero;
    }
};


static int Pagerank(ConfigData& conf) {
    MaiterKernel<int, float, vector<int> >* kernel = new MaiterKernel<int, float, vector<int> >(
                                        conf, FLAGS_num_nodes, FLAGS_portion, FLAGS_result_dir,
                                        new Sharding::Mod,
                                        new PagerankIterateKernel,
                                        new TermCheckers<int, float>::Diff);
    
    
    kernel->registerMaiter();

    if (!StartWorker(conf)) {
        Master m(conf);
        m.run_maiter(kernel);
    }
    
    delete kernel;
    return 0;
}

REGISTER_RUNNER(Pagerank);

```
# Single source shortest path #

```
#include "client/client.h"


using namespace dsm;

DECLARE_string(result_dir);
DECLARE_int64(num_nodes);
DECLARE_double(portion);
DECLARE_int64(shortestpath_source);


struct ShortestpathIterateKernel : public IterateKernel<int, float, vector<Link> > {
    float imax;

    ShortestpathIterateKernel(){
        imax = std::numeric_limits<float>::max();
    }

    void read_data(string& line, int& k, vector<Link>& data){
        string linestr(line);
        int pos = linestr.find("\t");
        int source = boost::lexical_cast<int>(linestr.substr(0, pos));

        vector<Link> linkvec;
        int spacepos = 0;
        string links = linestr.substr(pos+1);
        while((spacepos = links.find_first_of(" ")) != links.npos){
            Link to(0, 0);

            if(spacepos > 0){
                string link = links.substr(0, spacepos);
                int cut = links.find_first_of(",");
                to.end = boost::lexical_cast<int>(link.substr(0, cut));
                to.weight = boost::lexical_cast<float>(link.substr(cut+1));
            }
            links = links.substr(spacepos+1);
            linkvec.push_back(to);
        }

        k = source;
        data = linkvec;
    }

    void init_c(const int& k, float& delta,vector<Link>& data){
        if(k == FLAGS_shortestpath_source){
            delta = 0;
        }else{
            delta = imax;
        }
    }
    void init_v(const int& k,float& v,vector<Link>& data){
        v = imax;  
    }
        
    void accumulate(float& a, const float& b){
        a = std::min(a, b); 
    }

    void priority(float& pri, const float& value, const float& delta){
        pri = value - std::min(value, delta); 
    }

    void g_func(const int &k, const float& delta,const float& value, const vector<Link>& data, vector<pair<int, float> >* output){
        for(vector<Link>::const_iterator it=data.begin(); it!=data.end(); it++){
            Link target = *it;
            float outv = delta + target.weight;
            output->push_back(make_pair(target.end, outv));
        }
    }

    const float& default_v() const {
        return imax;
    }
};


static int Shortestpath(ConfigData& conf) {
    MaiterKernel<int, float, vector<Link> >* kernel = new MaiterKernel<int, float, vector<Link> >(
                                        conf, FLAGS_num_nodes, FLAGS_portion, FLAGS_result_dir,
                                        new Sharding::Mod,
                                        new ShortestpathIterateKernel,
                                        new TermCheckers<int, float>::Diff);
    
    
    kernel->registerMaiter();

    if (!StartWorker(conf)) {
        Master m(conf);
        m.run_maiter(kernel);
    }
    
    delete kernel;
    return 0;
}

REGISTER_RUNNER(Shortestpath);


```


# Adsorption #

```
#include "client/client.h"


using namespace dsm;

DECLARE_string(result_dir);
DECLARE_int64(num_nodes);
DECLARE_double(portion);
DECLARE_int32(adsorption_starts);
DECLARE_double(adsorption_damping);


struct AdsorptionIterateKernel : public IterateKernel<int, float, vector<Link> > {
    
    float zero;

    AdsorptionIterateKernel() : zero(0){}

    void read_data(string& line, int& k, vector<Link>& data){
        string linestr(line);
        int pos = linestr.find("\t");
        int source = boost::lexical_cast<int>(linestr.substr(0, pos));

        vector<Link> linkvec;
        int spacepos = 0;
        string links = linestr.substr(pos+1);
        while((spacepos = links.find_first_of(" ")) != links.npos){
            Link to(0, 0);

            if(spacepos > 0){
                string link = links.substr(0, spacepos);
                int cut = links.find_first_of(",");
                to.end = boost::lexical_cast<int>(link.substr(0, cut));
                to.weight = boost::lexical_cast<float>(link.substr(cut+1));
            }
            links = links.substr(spacepos+1);
            linkvec.push_back(to);
        }

        k = source;
        data = linkvec;
    }

    void init_c(const int& k, float& delta,vector<Link>& data){
        if(k < FLAGS_adsorption_starts){
            delta = 10;
        }else{
            delta = 0;
        }
    }
    void init_v(const int& k, float& delta,vector<Link>& data){
        delta=zero;
    }
    void accumulate(float& a, const float& b){
        a = a + b;
    }

    void priority(float& pri, const float& value, const float& delta){
        pri = delta;
    }

    void g_func(const int& k, const float& delta,const float& value, const vector<Link>& data, vector<pair<int, float> >* output){
        for(vector<Link>::const_iterator it=data.begin(); it!=data.end(); it++){
            Link target = *it;
            float outv = delta * FLAGS_adsorption_damping * target.weight;
            output->push_back(make_pair(target.end, outv));
        }
    }

    const float& default_v() const {
        return zero;
    }
};

static int Adsorption(ConfigData& conf) {
    MaiterKernel<int, float, vector<Link> >* kernel = new MaiterKernel<int, float, vector<Link> >(
                                        conf, FLAGS_num_nodes, FLAGS_portion, FLAGS_result_dir,
                                        new Sharding::Mod,
                                        new AdsorptionIterateKernel,
                                        new TermCheckers<int, float>::Diff);
    
    
    kernel->registerMaiter();

    if (!StartWorker(conf)) {
        Master m(conf);
        m.run_maiter(kernel);
    }
    
    delete kernel;
    return 0;
}

REGISTER_RUNNER(Adsorption);



```


# Katz metric #

```
#include "client/client.h"


using namespace dsm;

DECLARE_string(result_dir);
DECLARE_int64(num_nodes);
DECLARE_double(portion);
DECLARE_int64(katz_source);
DECLARE_double(katz_beta);
DECLARE_int32(shards);


struct KatzIterateKernel : public IterateKernel<int, float, vector<int> > {
    
    float zero;

    KatzIterateKernel() : zero(0){}

    void read_data(string& line, int& k, vector<int>& data){
        string linestr(line);
        int pos = linestr.find("\t");
        int source = boost::lexical_cast<int>(linestr.substr(0, pos));

        vector<int> linkvec;
        string links = linestr.substr(pos+1);
        int spacepos = 0;
        while((spacepos = links.find_first_of(" ")) != links.npos){
            int to;
            if(spacepos > 0){
                to = boost::lexical_cast<int>(links.substr(0, spacepos));
            }
            links = links.substr(spacepos+1);
            linkvec.push_back(to);
        }

        k = source;
        data = linkvec;
    }

    void init_c(const int& k, float& delta,vector<int>& data){
        if(k == FLAGS_katz_source){
            delta = 1000000;
        }else{
            delta = 0;
        }
    }

    void init_v(const int& k, float& delta,vector<int>&data){
        delta=zero;
    }
    void accumulate(float& a, const float& b){
        a = a + b;
    }

    void priority(float& pri, const float& value, const float& delta){
        pri = delta;
    }

    void g_func(const int& k,const float& delta, const float& value, const vector<int>& data, vector<pair<int, float> >* output){
        float outv = FLAGS_katz_beta * delta;
        for(vector<int>::const_iterator it=data.begin(); it!=data.end(); it++){
            int target = *it;
            output->push_back(make_pair(target, outv));
        }
    }

    const float& default_v() const {
        return zero;
    }
};


static int Katz(ConfigData& conf) {
    MaiterKernel<int, float, vector<int> >* kernel = new MaiterKernel<int, float, vector<int> >(
                                        conf, FLAGS_num_nodes, FLAGS_portion, FLAGS_result_dir,
                                        new Sharding::Mod,
                                        new KatzIterateKernel,
                                        new TermCheckers<int, float>::Diff);
    
    
    kernel->registerMaiter();

    if (!StartWorker(conf)) {
        Master m(conf);
        m.run_maiter(kernel);
    }
    
    delete kernel;
    return 0;
}

REGISTER_RUNNER(Katz);



```

# SimRank #
```
#include "client/client.h"


using namespace dsm;

DECLARE_string(result_dir);
DECLARE_int64(num_nodes);
DECLARE_double(portion);

struct Simrankiterate : public IterateKernel<string, double, vector<vector<int> > > {
    double zero;
    int count;
    Simrankiterate() : zero(0){count = 0;}

    void read_data(string& line, string& k, vector<vector<int> >& data){
        string linestr(line);
        int pos = linestr.find("\t");
        if(pos == -1) return;
        k=linestr.substr(0,pos);
        vector<vector<int> > linkvec;
        string remain=linestr.substr(pos+1);
        int pos1=remain.find(" ");
        string I_ab=remain.substr(0,pos1);
        int i_ab=boost::lexical_cast<int>(I_ab);
        vector<int> tmp;
        tmp.push_back(i_ab);
        linkvec.push_back(tmp);
        pos=pos1;
        remain=remain.substr(pos+1);
        pos1=remain.find("\t");
        string links_a=remain.substr(0,pos1);
        string links_b=remain.substr(pos1+1);
        if(links_a==""||links_b==""||links_a==" "||links_b==" "){
            data = linkvec;
            return;
        }
        int spacepos = 0;
        vector<int> tmp_a;
        while((spacepos = links_a.find_first_of(" ")) != links_a.npos){
            int to;
            if(spacepos > 0){
                to = boost::lexical_cast<int>(links_a.substr(0, spacepos));
            }
            links_a= links_a.substr(spacepos+1);
            tmp_a.push_back(to);
        }
        linkvec.push_back(tmp_a);
        spacepos = 0;
        vector<int> tmp_b;
        while((spacepos = links_b.find_first_of(" ")) != links_b.npos){
            int to;
            if(spacepos > 0){
                to = boost::lexical_cast<int>(links_b.substr(0, spacepos));
            }
            links_b = links_b.substr(spacepos+1);
            tmp_b.push_back(to);
        }
        linkvec.push_back(tmp_b);
        data = linkvec;
    }

    void init_c(const string& k, double& delta,vector<vector<int> >& data){
            string key=k;
            int pos=key.find("_");
            string key_a=key.substr(0,pos);
            string key_b=key.substr(pos+1);
            if(key_a==key_b){
                delta=data[0][0]/0.8; 
            }else{
                delta=0;
            }
                
    } 
    void init_v(const string& k,double& v,vector<vector<int> >& data){
            v=0.0;
    }
    void process_delta_v(const string& k, double& delta, double& value, vector<vector<int> >& data){
        if(delta==0) return;
        int I_ab=boost::lexical_cast<int>(data[0][0]);
        if(I_ab==0)return;  
        delta=(delta*0.8)/I_ab;
    }

    void accumulate(double& a, const double& b){
            a = a + b;
    }

    void priority(double& pri, const double& value, const double& delta){
            pri = delta;
    }

    void g_func(const string& k, const double& delta,const double&value, const vector<vector<int> >& data, vector<pair<string, double> >* output){
            if(data.size()<3){
                return;
            }
            string key=k;
            count++;
            if(delta==0) return;
            int I_ab=data[0][0];
            if(I_ab==0)return;
            double outv = delta;
            int size_a=data[1].size();
            int size_b=data[2].size();
            if(size_a==0||size_b==0) return;
            vector<string> list;
            for(vector<int>::const_iterator it_a=data[1].begin(); it_a!=data[1].end(); it_a++){
                for(vector<int>::const_iterator it_b=data[2].begin(); it_b!=data[2].end(); it_b++){
                    int a= *it_a;
                    int b= *it_b;
                    string key_a=boost::lexical_cast<string>(a);
                    string key_b=boost::lexical_cast<string>(b);
                    string key;
                    if(a==b) continue;
                    if(a<b){
                        key=key_a+"_"+key_b;
                    }else {
                        key=key_b+"_"+key_a;
                    }
                    int pos=1;
                    for(vector<string>::const_iterator it=list.begin(); it!=list.end(); it++){
                        string k=*it;
                        if(k==key){
                            pos=0;
                            break;
                        }
                    }
                    if(pos==1){
                        list.push_back(key);
                        output->push_back(make_pair(key,outv));
                    }     
                }
            } 
    }
   
    const double& default_v() const {
            return zero;
    }
};
  struct SUM : public TermChecker<string, double> {
    double last;
    double curr;
    
    SUM(){
        last = -std::numeric_limits<double>::max();
        curr = 0;
    }

    double set_curr(){
        return curr;
    }
    
    double estimate_prog(LocalTableIterator<string, double>* statetable){
        double partial_curr = 0;
        double defaultv = statetable->defaultV();
        while(!statetable->done()){
            bool cont = statetable->Next();
            if(!cont) break;
            if(statetable->value2() != defaultv){
                partial_curr += static_cast<double>(statetable->value2());
            }
        }
        return partial_curr;
    }
    
    bool terminate(vector<double> local_reports){
        curr = 0;
        vector<double>::iterator it;
        for(it=local_reports.begin(); it!=local_reports.end(); it++){
                curr += *it;
        }
        
        VLOG(0) << "terminate check : last progress " << last << " current progress " << curr << " difference " << abs(curr-last);
        VLOG(0)<<"FLAGS_termcheck_threshold: "<<FLAGS_termcheck_threshold;
        if(abs(curr) >= FLAGS_termcheck_threshold){
            //VLOG(0)<<"FLAGS_termcheck_threshold: "<<FLAGS_termcheck_threshold << "currtrue: "<< curr;
            return true;
        }else{
            last = curr;
            //VLOG(0)<<"FLAGS_termcheck_threshold: "<<FLAGS_termcheck_threshold << "curr: "<< curr;
            return false;
        }
    }
  };
static int Simrank(ConfigData& conf) {
    MaiterKernel<string, double, vector<vector<int> > >* kernel = new MaiterKernel<string, double, vector<vector<int> > >(
                                        conf, FLAGS_num_nodes, FLAGS_portion, FLAGS_result_dir,
                                        new Sharding::Mod_str,
                                        new Simrankiterate,
                                        new SUM);
    
    
    kernel->registerMaiter();

    if (!StartWorker(conf)) {
        Master m(conf);
        m.run_maiter(kernel);
    }
    
    delete kernel;
    return 0;
}

REGISTER_RUNNER(Simrank);


```

# Connected Componentment #

```

#include "client/client.h"


using namespace dsm;

//DECLARE_string(graph_dir);
DECLARE_string(result_dir);
DECLARE_int64(num_nodes);
DECLARE_double(portion);

struct PagerankIterateKernel : public IterateKernel<int, int, vector<int> > {
    int zero;


    PagerankIterateKernel() : zero(0){}

    void read_data(string& line, int& k, vector<int>& data){
        string linestr(line);
        int pos = linestr.find("\t");
        if(pos == -1) return;
        
        int source = boost::lexical_cast<int>(linestr.substr(0, pos));

        vector<int> linkvec;
        string links = linestr.substr(pos+1);
        int spacepos = 0;
        while((spacepos = links.find_first_of(" ")) != links.npos){
            int to;
            if(spacepos > 0){
                to = boost::lexical_cast<int>(links.substr(0, spacepos));
            }
            links = links.substr(spacepos+1);
            linkvec.push_back(to);
        }

        k = source;
        data = linkvec;
    }

    void init_c(const int& k, int& delta, vector<int>& data){
            int  init_delta = k;
            delta = init_delta;
    }

    void init_v(const int& k,int& v,vector<int>& data){
            v=0;
    }
    void accumulate(int& a, const int& b){
            a=std::max(a,b);
    }

    void priority(int& pri, const int& value, const int& delta){
            pri = value-std::max(value,delta);
    }

    void g_func(const int& k,const int& delta, const int& value, const vector<int>& data, vector<pair<int, int> >* output){
            int outv = value;
            
            //cout << "size " << size << endl;
            for(vector<int>::const_iterator it=data.begin(); it!=data.end(); it++){
                    int target = *it;
                    output->push_back(make_pair(target, outv));
            }
    }

    const int& default_v() const {
        return zero;
    }
};


static int Pagerank(ConfigData& conf) {
    MaiterKernel<int, int, vector<int> >* kernel = new MaiterKernel<int, int, vector<int> >(
                                        conf, FLAGS_num_nodes, FLAGS_portion, FLAGS_result_dir,
                                        new Sharding::Mod,
                                        new PagerankIterateKernel,
                                        new TermCheckers<int, int>::Diff);
    
    
    kernel->registerMaiter();

    if (!StartWorker(conf)) {
        Master m(conf);
        m.run_maiter(kernel);
    }
    
    delete kernel;
    return 0;
}

REGISTER_RUNNER(Pagerank);


```