
```
#include "client/client.h"

using namespace dsm;

DECLARE_string(result_dir);
DECLARE_int64(num_nodes);
DECLARE_double(portion);

struct PageRankPartitioner : public Partitioner<int,vector<int> >{

    void parse_line(string& line, int* k, vector<int>* data) {
        string linestr(line);
        int pos = linestr.find("\t");
        int source = boost::lexical_cast<int>(linestr.substr(0, pos));

        vector<int> linkvec;
        string links = linestr.substr(pos+1);
        int spacepos = 0;
        while((spacepos = links.find_first_of(" ")) != links.npos){
            int to;
            if(spacepos > 0){
                to = boost::lexical_cast<int>(links.substr(0, pacepos));
            }
            links = links.substr(spacepos+1);
            linkvec.push_back(to);
        }

        *k = source;
        *data = linkvec;
    }

    int partition(const int& key, int shards) {
        return key % shards; 
    }
}

struct PagerankIterateKernel : public IterateKernel<int, float, vector<int> > {

    void init(const int& k, float* delta){
        float init_delta = 0.2;
        *delta = init_delta;
    }

    void accumulate(float* a, const float& b){
        *a = *a + b;
    }

    void send(const float& delta, const vector<int>& data, vector<pair<int, float> >* output){
        int size = (int) data.size();
        float outdelta = delta * 0.8 / size;
        for(vector<int>::const_iterator it=data.begin(); it!=data.end(); it++){
            int target = *it;
            output->push_back(make_pair(target, outdelta));
        }
    }
}

struct PageRankTermChecker : public TermChecker<int, float> {

    double prev_prog = 0.0;
    double curr_prog = 0.0;

    double estimate_prog(LocalTableIterator<int, float>* statetable){
        double partial_curr = 0;
        V defaultv = statetable->defaultV();
        while(!statetable->done()){
            bool cont = statetable->Next();
            if(!cont) break;
            if(statetable->v() != defaultv){
                partial_curr += static_cast<double>(statetable->v());
            }
        }
        return partial_curr;
    }

    bool terminate(list<double> local_reports){
        list<string>::iterator prog_iter;
        for(prog_iter=local_reports.begin(); prog_iter != local_reports.end(); ++prog_iter){
            curr_prog += *prog_iter;
        }

        if(abs(curr_prog - prev_prog) < term_threshold){
            return true;
        }else{
            prev_prog = curr_prog;
            return false;
        }
    }
}
```