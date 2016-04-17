# Introduction #

The following 3 components should be implemented in Maiter. They are quite easy to implement after you have figured out what the commutative operator is and what the g_{i,j}(x) function is._

  * `Sharder`
  * `IterateKernel`
  * `TermChecker`

# API #

```
template <class K>
struct Sharder : public SharderBase {
    virtual int operator()(const K& k, int shards) = 0;
};

template <class K, class V, class D>
struct IterateKernel : public IterateKernelBase {

  virtual void read_data(string& line, K& k, D& data) = 0;//Initailizing the data domain ofstatetable
  virtual void init_c(const K& k, V& delta,D& data) = 0;// Initailizing the dlate_v domaim of statetable
  virtual const V& default_v() const = 0;//Setting the "0" which satisfes the x ⊕"0"=x. ⊕is  operation such plus
  virtual void init_v(const K& k, V& v, D& data) = 0;//Initializing the V domain of statetable
  virtual void accumulate(V& a, const V& b) = 0;//Setting the manner of accumulating the delta_v,such as plus,Max(x,y),Min(x,y)
  virtual void process_delta_v(const K& k, V& dalta,V& value, D& data){}//Processing the value of delta_v domain of statetable 
  virtual void priority(V& pri, const V& value, const V& delta) = 0;//Setting the priority of statetable 
  virtual void g_func(const K& k,const V& delta,const V& value, const D& data, vector<pair<K, V> >* output) = 0;// Implementation of Update operation of Maiter
};


template <class K, class V>
struct TermChecker : public TermCheckerBase {
    virtual double estimate_prog(LocalTableIterator<K, V>* table_itr) = 0;
    virtual bool terminate(vector<double> local_reports) = 0;
};
```
For more usage detail of Maiter API, please refer to
<a href='http://code.google.com/p/maiter/wiki/Guidance#Implementation_of'>Implementation of PageRank</a>