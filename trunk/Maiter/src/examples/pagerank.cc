#include "client/client.h"


using namespace dsm;

//DECLARE_string(graph_dir);
DECLARE_string(result_dir);
DECLARE_int64(num_nodes);
DECLARE_double(portion);

struct PagerankIterateKernel : public IterateKernel<int, float, vector<int> > {
    float zero;

    PagerankIterateKernel() : zero(0){}

	void read_data(string& line, K* k, D* data){
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

	    *k = source;
	    *data = linkvec;
	}

	void init_c(const K& k, V* delta){
		float init_delta = 0.2;
		*delta = init_delta;
	}

	void accumulate(V* a, const V& b){
		*a = *a + b;
	}

	void priority(V* pri, const V& value, const V& delta){
		*pri = delta;
	}

	void g_func(const V& delta, const D& data, vector<pair<K, V> >* output){
		int size = (int) data.size();
		float outv = delta * 0.8 / size;
		for(vector<int>::const_iterator it=data.begin(); it!=data.end(); it++){
			int target = *it;
			output->push_back(make_pair(target, outv));
		}
	}

	const V& default_v() const {
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
