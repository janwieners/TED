// layerk.h		V.Rao, H. Rao
// header file for the Kohonen layer and
// the Kohonen network
#include "layerforkohonen.h"
#include <QString>
                                
class Kohonen_network;

class Kohonen_layer: public layer
{
protected:
	float * weights;
	int winner_index;
	float win_distance;
	int neighborhood_size;
	
	friend class Kohonen_network;  
	
public:
	Kohonen_layer(int, int, int);
	~Kohonen_layer();    
	virtual void calc_out();
	void randomize_weights();
	void update_neigh_size(int);
	void update_weights(const float);
	void list_weights();
	void list_outputs();
	float get_win_dist();
}; 


class Kohonen_network 
{
private:
	layer *layer_ptr[2];
    int layer_size[2];
    int neighborhood_size;
     
public:
    Kohonen_network();
    ~Kohonen_network();
	void get_layer_info(int inputlayersize, int outputlayersize);
	void set_up_network(int);
	void randomize_weights();
	void update_neigh_size(int);
	void update_weights(const float);
	void list_weights();
	void list_outputs();       
	void get_next_vector(float *inputvector);
	void process_next_pattern();
	float get_win_dist();
	int get_win_index();
};


