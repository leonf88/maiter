  * **Guigance**
    * <a href='http://code.google.com/p/maiter/wiki/Guidance#Running_Maiter_on_single_machine'>Running Maiter on single machine</a>
    * <a href='http://code.google.com/p/maiter/wiki/Guidance#Running_Maiter_on_a_cluster'>Runing Maiter on cluster</a>
    * <a href='http://code.google.com/p/maiter/wiki/Guidance#Programing_on_Maiter'>Programing on Maiter</a>
    * <a href='http://code.google.com/p/maiter/wiki/Guidance#Some_common_problems'>Somecommon problems</a>


# **Running Maiter on single machine** #
If you don’t want to run a big task, you just intend to run a simple example like PageRank to have a try or learn how to use Maiter. You don’t need to set up Maiter on big cluster. Setting up Maiter on single machine will save a lot of time. According to the following steps, you can run Maiter on single machine easily.
## **1. prerequisites** ##
Maiter is C++ framework, therefore to build and use Maiter, you will need a minimum of the following:
  * CMake (> 2.6)
  * OpenMPI
  * Python (2.**)
  * gcc/g++ (> 4)
  * protocol buffers
If available, the following libraries will be used:
  * Python development headers; SWIG
  * TCMalloc
  * google-perftools
In addition to these, Maiter comes with several support libraries which are compiled as part of the build process; these are:
  * google-flags
  * google-logging
On debian/ubuntu, the required libraries can be acquired by running:
  * sudo apt-get install build-essential cmake g++ libboost-dev libboost-python-dev libboost-thread-dev liblzo2-dev libnuma-dev libopenmpi-dev libprotobuf-dev libcr-dev libibverbs-dev openmpi-bin protobuf-compiler liblapack-dev libboost-system-dev**

the optional libraries can be install via:
  * sudo apt-get install libgoogle-perftools-dev python-dev swig


## **2. Building** ##
To build, simply run 'make' from the top level Maiter directory.

![http://fmn.rrimg.com/fmn058/20130902/2120/original_IFoY_71a000007587125d.jpg](http://fmn.rrimg.com/fmn058/20130902/2120/original_IFoY_71a000007587125d.jpg)

A successful building will show below in the end:

![http://fmn.rrimg.com/fmn064/20130902/2120/original_qBnZ_09bf00007e491190.jpg](http://fmn.rrimg.com/fmn064/20130902/2120/original_qBnZ_09bf00007e491190.jpg)

A successful build should generate an executable example case named example-dsm which is PageRank in default situation in the bin/release/examples/ of Maiter project.
## **3. Running** ##
**Firstly,** to execute the default example (it is PageRank in default situation) generated after building, you will need to modify conf/mpi-cluster to point to your machine Maiter will be executed on - for example, the file might look like:
```
localhost slots=1
localhost slots=1
```
The first line is master and the second line is worker in your single-machine cluster. Master and worker are your machine so that your simple cluster can run on single machine.
The slots parameter is that how many Maiter tasks run at the same time on one worker. Setting slots parameter has some tricks. The best value of slots is the number of cores in your CPU.
**Secondly,** you have to write your own .sh file to set parameters for default example. For example, if the example is PageRank, your .sh file might look like:
```
ALGORITHM=Pagerank
WORKERS=2
GRAPH=input/
RESULT=result/
NODES=1000
SNAPSHOT=10
TERMTHRESH=0.00001
BUFMSG=1
PORTION=1
./exsmple-dsm --runner=$ALGORITHM --workers=$WORKERS --graph_dir=$GRAPH --result_dir=$RESULT --num_nodes=$NODES --snapshot_interval=$SNAPSHOT --portion=$PORTION --termcheck_threshold=$TERMTHRESH --bufmsg=$BUFMSG --v=1 > log
```
> In the .sh file, there are many parameters which have specific meanings:
  * ALGORITHM is the name of algorithm and must be exactly the same with the name you set when implementing the algorithm.
  * WORKERS is the sum of the values of slots in the conf/mpi-cluster file.
  * GRAPH is the relative path of the input file like graph.
  * RESULT is the relative path of results of the algorithm you run.
  * NODES is the number of the statetable previsioned in memory before run a task and depends on the number of nodes in the input graph. For example, supposed that there are 1000 nodes in your input graph, the NODES must be bigger than 1000. Therefore, it would be 1500.
  * SNAPSHOT is the time interval (in seconds) between two snapshots.
  * TERMTHRESH is a float number and can be any values which depend on the implementation of terminate API of Maiter.
  * BUFMSG is the size of buffer which caches information to be sent by workers.
  * PORTION determines that how many statetables are preferentially updated. It ranges from 0 to 1. For example, if the PORTION is 0.2, Maiter will preferentially update 20 percent of statetables which have higher priority.

**Finally,** you must put the example-dsm, .sh file, input (note that the name of input file in input directory must be ‘part0’ ) and result directory in the same directory. Then entering the command ‘sh .sh’, you will get output in the result directory.

![http://fmn.rrimg.com/fmn062/20130902/2120/original_9Z55_6bcb00007e2d1191.jpg](http://fmn.rrimg.com/fmn062/20130902/2120/original_9Z55_6bcb00007e2d1191.jpg)

Figure 1

# **Running Maiter on a cluster** #
If you want to run a big task on Maiter, single machine cluster will be not enough. But, setting up Maiter on single machine with easy, will make you bring you a lot of convenience, because there are many similarities between single machine cluster and multiple machines cluster.
## **1. Prerequisites** ##
It is similar with the first step of single machine cluster, but you must do this step on every machine in cluster.
## **2. Building** ##
It is similar with the second step of single machine cluster, but you must do this step on every machine in cluster.
## **3. Running** ##
Firstly, you also need to modify the conf/mpi-cluster file to inform Maiter that how many machines in your cluster and how to communicate with them. In multiple machines cluster, the file might look like:
```
192.168.0.1  slots=1
192.168.0.2  slots=2
192.168.0.3  slots=2
192.168.0.4  slots=2
192.168.0.5  slots=2
```
Reading this file, Maiter will know that there are 5 machines in cluster which include 1 master and 4 workers, the master’s IP is 192.168.0.1, the first worker’s IP is 192.168.0.2, only two Maiter tasks can be run at the same time on one worker.
**Secondly,** if the task you want to run is PageRank, your .sh file would look like:
```
ALGORITHM=Pagerank
WORKERS=9
GRAPH=input/
RESULT=result/
NODES=1000
SNAPSHOT=10
TERMTHRESH=0.00001
BUFMSG=1
PORTION=1
./exsmple-dsm --runner=$ALGORITHM --workers=$WORKERS --graph_dir=$GRAPH --result_dir=$RESULT --num_nodes=$NODES --snapshot_interval=$SNAPSHOT --portion=$PORTION --termcheck_threshold=$TERMTHRESH --bufmsg=$BUFMSG --v=1 > log
```
**Thirdly,** you must preprocess your input file. At present, Maiter has no distributed file system to save and split input file. Therefore, to begin your task, you must split the input file into corresponding parts with hash function which must be the same with the hash function you set in implementation of your task (it is key mod WORKS-1 in default situation). The number of parts depends on the WORKS parameter in your .sh file. It is WORKS-1. For example, WORKS is 9 in above .sh file. The process of splitting input file be shown in figure 2.

> ![http://fmn.rrimg.com/fmn059/20130902/2120/original_Cepn_0c5300007e29118f.jpg](http://fmn.rrimg.com/fmn059/20130902/2120/original_Cepn_0c5300007e29118f.jpg)

> Figure 2

In figure 1.2, you should pay attention to the name of every part. If the hash value of key in a part is x, the name of the part must be partx. After splitting, you have to past these parts to corresponding worker, then putting them in the input directory. Because a worker can run two Maiter tasks at the same time, every worker should has two parts of input file. Shown in figure 3 is how to place the example-dsm, parts, input and result directory.


> ![http://fmn.rrimg.com/fmn061/20130902/2120/original_0V3g_0af300007e0a1190.jpg](http://fmn.rrimg.com/fmn061/20130902/2120/original_0V3g_0af300007e0a1190.jpg)
> > Figure 3

In figure 1.3, you should note that part0 and part1 must be placed in worker0 , part2 and part3 must be placed in worker1 and so on. The second line in cong/map-cluster is worker0, the third line is worker1. The rest can be done in the same manner. Besides, you have to put the executable example-dsm, Maiter project, input and result in a directory which is the same on all machines in your cluster.
**Finaly,** running the .sh file on master (192.168.0.1), you can get a part of the end result in the result directory on a worker. Then collecting the part on every worker, you will get the final result.

# **Programing on Maiter** #
**Firstly,** program your .cc file. Before programing on Maiter, you should understand well how Maiter works. You can read the paper on google code of Maiter for detail.
Actually, the running process of Maiter include there steps. In the first step, Maiter read the input file to initialize statetables on every worker, preparing for computing in nest step. After all statetables of workers have been initialized, Maiter start the second step, iteratively computing to get results with input file until the results meet specific precision. In the third step, Maiter dumps the results on disks of workers. This is the end of your task. The running process is shown in figure 4.

![http://fmn.rrimg.com/fmn063/20130902/2120/original_wDhp_45b400007e12118e.jpg](http://fmn.rrimg.com/fmn063/20130902/2120/original_wDhp_45b400007e12118e.jpg)


> Figure 4

Maiter can finish the third step without your interference. Therefore, you only need to finish the former two steps with the API provided by Maiter. Take PageRank for example. The C++ code of PageRank in Maiter programming model is shown below:

## Implementation of PageRank ##
```
#include "client/client.h"
using namespace dsm;

DECLARE_string(result_dir);
DECLARE_int64(num_nodes);
DECLARE_double(portion);
//These are google flag functions. They turn some variables Maiter require such as result //directory into command line arguments to make running tasks convenient.
struct PagerankIterateKernel : public IterateKernel<int, float, vector<int> > {
    float zero;
PagerankIterateKernel() : zero(0){} // constructor of PagerankIterateKernel
    *void read_data(string& line, int& k, vector<int>& data){ 
        string linestr(line);
        int pos = linestr.find("\t");
        if(pos == -1) return;
        int source = boost::lexical_cast<int>(linestr.substr(0, pos));
        vector<int> linkvec;
        string links = linestr.substr(pos+1);
        cout<<"links:"<<links<<endl;
        int spacepos = 0;
        while((spacepos = links.find_first_of(" ")) != links.npos){
            int to;
            if(spacepos > 0){
                to = boost::lexical_cast<int>(links.substr(0, spacepos));
                cout<<"to:"<<to<<endl;
            }
            links = links.substr(spacepos+1);
            linkvec.push_back(to);
        }
        k = source;
        data = linkvec;
    }
// read_data function initializes domain data of statetable, corresponding to operation 4 // in figure 4
    void init_c(const int& k, float& delta,vector<int>& data){ 
            float init_delta = 0.2;
            delta = init_delta;
   }
// init_c function initializes domain Δv of statetable，corresponding to operation 2 in //figure 4
   void init_v(const int& k,float& v,vector<int>& data){ 
        v=0;
}
// init_v function initializes domain v of statetable，corresponding to operation 1 
    void accumulate(float& a, const float& b){ 
            a = a + b;
    }
// accumulate function informs Maiter the manner of accumulation in the operation 7 //and operation 8 in figure 4
    *void priority(float& pri, const float& value, const float& delta){ *
            pri = delta;
    }
// init_v function initializes domain pri of statetable，corresponding to operation 3
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
// g_func function computes messages to be sent with the data in domain data and then // sends these massages to adjacent nodes, corresponding to the operation 9
static int Pagerank(ConfigData& conf) {
MaiterKernel<int, float, vector<int> >* kernel = new MaiterKernel<int, float, vector<int> > (
conf, // It is conf/map_cluster in default situation. You don’t need to change it.
FLAGS_num_nodes, 
FLAGS_portion,
FLAGS_result_dir,
new Sharding::Mod,// Setting the hash function used to find corresponding statetable on // worker with a key. You can redefined it. 
new PagerankIterateKernel,
new TermCheckers<int, float>::Diff // Setting the manner of stopping the task. You can //redefined it with API terminate and estimate_prog.
);
    kernel->registerMaiter();
    if (!StartWorker(conf)) {
        Master m(conf);
        m.run_maiter(kernel);
    }
    delete kernel;
    return 0;
}
// stating the task
REGISTER_RUNNER(Pagerank); // Setting the parameter RUNNER in .sh file
```
**Secondly,** name your code file .cc, for example, PageRank.cc and put PagRank.cc into src/examples/ directory of Maiter project. Then you need to modify the CMAKELIST.TXT file in src/examples/ directory.
```
set(EXAMPLE_LIBS 
	${EXAMPLE_LIBS} 
	${MPI_LIBS}
	${BLAS_LIBS}
  	${PROTOBUF_LIBS})  	  	
set(EXAMPLE_LIBS ${EXAMPLE_LIBS} boost_thread-mt util z lzo2 pthread rt)
add_custom_target(example_proto DEPENDS ${EXAMPLE_PB_HDR})
add_library(example 
            PageRank.cc
            con_component.cc
            simrank.cc
            shortestpath.cc
            adsorption.cc
            katz.cc
            ${EXAMPLE_PB_HDR} 
            ${EXAMPLE_PB_SRC})
add_dependencies(example worker_proto common_proto)
add_executable(example-dsm main.cc)
# we need to resolve static initializers, so glob all the symbols together with -whole-archive
target_link_libraries(example-dsm -Wl,-whole-archive common worker kernel master example -Wl,-no-whole-archive)
target_link_libraries(example-dsm gflags glog webgraph)
target_link_libraries(example-dsm ${EXAMPLE_LIBS})
```
**Finally**, run 'make' from the top level Maiter directory. The example-dsm in bin/release/examples/ directory is the executable PageRank. Then posting it to all the workers in your cluster and running your ,sh file after input file is ready, you will get results quickly.

# Some common problems #
## No entry for requested key ##
When ruing your won apps, you might encounter the problem like below:

> ![http://fmn.rrimg.com/fmn060/20131006/2010/original_8XCu_60d000008d42118f.jpg](http://fmn.rrimg.com/fmn060/20131006/2010/original_8XCu_60d000008d42118f.jpg)

“No entry for requested key 2\_4” means that there isn’t a entry whose key domain is 2\_4 in statetable in memory. Maiter initializes the statetable when your job begin. Every row in your input data corresponds to a entry which is labeled by unique key in statetable. when the job is running, for example, key 3\_2 might sends massages to key 2\_4. If the key 2\_4 has no a entry in statetable, Maiter will cannot find the target to send its massages, then Maiter will report the error and exit.
To settle the problem, you must make sure the part of your input data that will be keys of entries includes all the keys appearing in the process of your job. Then run your job again.