// my patches to sha1
// I have overwritten (replaced) sha1.h

#include "sha1.cpp"

//using namespace std;

//	Don't have to worry about end case
// Just stream the bits from the files in 64 byte blocks

# define buffer_size 1
int SHA1::update_hashpiece(std::ifstream &is, int piecelength, int &fileindex, std::vector<string> &filelist){
	char bigbuf[piecelength];
	is.read(bigbuf,piecelength);
	buffer.append(bigbuf, is.gcount());
	
	while(buffer.size()!=piecelength){
		
		is.close();				
		if (++fileindex==filelist.size()) break;
		is.open(filelist[fileindex], std::ios::binary);
		if(!is.is_open()) return 1;
		is.read(bigbuf, piecelength - buffer.size());
		buffer.append(bigbuf, is.gcount());
	}
	
	
	uint32_t block[BLOCK_INTS];
	while ((transforms+1)*BLOCK_BYTES<=buffer.size()){
		buffer_to_block(buffer.substr(transforms*BLOCK_BYTES, BLOCK_BYTES), block);
		transform(digest, block, transforms);
	}
	buffer=buffer.substr(transforms*BLOCK_BYTES);
//	assert (buffer.size()<BLOCK_BYTES);
//	return buffer.size()>=BLOCK_BYTES;
	return 0;
}


//int main(int argc, char *argv[]){

//}
	
	
	
