#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <assert.h>
#include "sha1.hpp"
#include "mysha1.cpp"
#define verbosedecode 0
#define print_error_bad_torrent 1
#define debugprint 0
#define how_many_bytes 10

using namespace std;

string path(string n){
	char backslash=0x5c;
	std::size_t slashpos = n.rfind(backslash);
	if (slashpos==string::npos)		return "";
	else 							return n.substr(0,slashpos);}	//does not include the slash at end
string filename(string n){
	char backslash=0x5c;
	std::size_t slashpos = n.rfind(backslash);
	if (slashpos==string::npos)		return n;
	else 							return n.substr(slashpos+1);}	// does not include slashes
void printvector(vector<string> &v){
	for (int i=0;i<v.size();i++)
		cout<<v[i]<<endl;
}
void printvectorskipslash(vector<string> &v){
	for (int i=0;i<v.size();i++)
		cout<<v[i].substr(1)<<endl;
}
bool is_digit(char n){
	return ('0'<=n and n<='9');}
string indent(int n){
	return string(4*n,' ');
}
string hex(string n){
	string return_string;
	char hex[]="0123456789abcdef";
	for (int i=0;i<how_many_bytes;i++){
		return_string.append(1,hex[(n[i] & 0xf0)>>4]);
		return_string.append(1,hex[(n[i] & 15)]);
	}
	return return_string;
}
int string_to_int(string n){
	istringstream instr(n);
	int output;
	instr >> output;
	return output;
}
// There are two functions that are similarily named.
// Because the same functionality is needed to decode strings
// for map keys I have two functions this decodes the string and outputs in in r
// and the other takes that r and saves it in the btitem
int string_decode_string(ifstream &n,int depth, string &r){
	string str;
	char ch;
	n.get(ch);
	if(!(is_digit(ch)))		return 1;
	while(is_digit(ch)){
		str.append(1,ch);
		n.get(ch);
	}
	if(!(ch==':'))			return 1;
	int size(string_to_int(str));
	for (int i=0;i<size;i++){
		n.get(ch);
		r.append(1,ch);
	}
	if (verbosedecode){
		if(r.size()<200)	cout<<indent(depth)<<r<<endl;
		else 				cout<<indent(depth)<<hex(r)<<"... hashstring size()="<<r.size()<<"\n";}
	return 0;
}
long long int string_to_long_int(string n){
	istringstream instr(n);
	long long int output;
	instr >> output;
	return output;
}
class btitem {
	public:
	btitem(){
		itemtype=0;
		depth=1;
		i=0;
	}
	btitem(const btitem &n){
		itemtype=n.itemtype;
		depth=n.depth;
		s=n.s;
		i=n.i;
		l=n.l;
		m=n.m;
	}
	
int decode_string(ifstream &n){
	itemtype=1;
	if (string_decode_string(n, depth, s)) return 1;
	return 0;
}
	
int decode_integer(ifstream &n){
	itemtype=2;
	string str;
	char ch;
	n.get(ch);
	if(!(ch=='i'))			return 1;
	n.get(ch);
	while ( '0' <= ch and ch <= '9' or ch=='-'){
		str.append(1,ch);
		n.get(ch);
	}
	if(!(ch=='e'))			return 1;
	i = string_to_long_int(str);
	if (verbosedecode)	cout<<indent(depth)<<i<<endl;
	return 0;
}
	
int decode_list(ifstream &n){
	itemtype=3;
	if (verbosedecode)	cout<<indent(depth)<<"list{\n";
	char ch;
	n.get(ch);
	if(!(ch=='l'))							return 1;
	while (n.peek()!='e'){
		btitem tempbtitem;
		tempbtitem.depth=depth+1;
		if (tempbtitem.decode_item(n))		return 1;
		l.push_back(tempbtitem);
	}
	n.get(ch);
	if(!(ch=='e'))							return 1;
	if (verbosedecode)	cout<<indent(depth)<<"}\n";
	return 0;
}
			
int decode_dictionary(ifstream &n){
	itemtype=4;
	if (verbosedecode)	cout<<indent(depth)<<"map{\n";
	char ch;
	n.get(ch);
	if(!(ch=='d'))			return 1;
	while (is_digit(n.peek())){		// could do not equal e for closing
		string tempstring;
		if (string_decode_string(n,depth+1,tempstring))	return 1;
		btitem tempbtitem;
		tempbtitem.depth=depth+1;
		if (tempbtitem.decode_item(n))					return 1;
			
		m[tempstring]=tempbtitem;
		if (verbosedecode)	cout<<endl;
	}
	n.get(ch);
	if(!(ch=='e'))			return 1;
	if (verbosedecode)	cout<<indent(depth)<<"}\n";
	return 0;
}
	
int decode_item(ifstream &input){
	switch (input.peek()){
		case 'd':
			return decode_dictionary(input);
			break;
		case 'i':
			return decode_integer(input);
			break;
		case 'l':
			return decode_list(input);
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return decode_string(input);
			break;
		default:
			return 1;
	}
}

int decode_item_from_file(const string &n){
	ifstream in(n, std::ios::binary);
	if(!in.is_open()) return 2;
	if (decode_item(in)) return 1;
	in.close();
	return 0;
}

void print(){
	switch (itemtype){
		case 1:
			if (s.size()<200)	cout<<indent(depth)<<s<<endl;
			else 	cout<<indent(depth)<<hex(s)<<"... #pieces= "<<s.size()/20<<"\n"<<endl;
			break;
		case 2:
			cout<<indent(depth)<<i<<endl;
			break;
		case 3:
			cout<<indent(depth)<<"list{\n";
			for (int i=0;i<l.size();i++)
				(l[i]).print();
			cout<<indent(depth)<<"}\n";
			break;
		case 4:
			cout<<indent(depth)<<"map{\n";
			for (std::map<string,btitem>::iterator mit=m.begin();mit!=m.end();mit++){
				cout<<indent(depth+1)<<mit->first<<endl;
				(mit->second).print();
				cout<<endl;
			}
			cout<<indent(depth)<<"}\n";
			break;
		case 0:
			assert(0);
			cout<<"Empty btitem\n";
		default :
			assert(0);
	}
}
	
vector<string> get_filenamelist(string dir){
	char slash[] = {0x5c, 0x00};
	vector<string> filenamelist;
	if (!m["info"].m.count("files")){
		filenamelist.push_back(dir+slash+m["info"].m["name"].s);
		return filenamelist;
	}
	for (int fn_i=0;fn_i<m["info"].m["files"].l.size();fn_i++){
		string r;
		r.append(dir);
		r.append(slash);
		vector<btitem> &paths = m["info"].m["files"].l[fn_i].m["path"].l;
		
		int size=paths.size();
		for (int i=0;i<size-1;i++){
			r.append(paths[i].s);
			r.append(slash);		
		}
		r.append(paths[size-1].s);
		filenamelist.push_back(r);
	}
	return filenamelist;
}

int get_piecelength(){
	return m["info"].m["piece length"].i;
}
int get_numberofpieces(){
	return m["info"].m["pieces"].s.size()/20;
}
vector<uint32_t> get_hashints(int index){
	vector<uint32_t> r;
	string &hash = m["info"].m["pieces"].s;
//	uint32_t a[5];
	int i=index*20;
	for (int j=0;j<5;j++)
    {
        r.push_back( (hash[j*4+i+3] & 0xff)
                   | (hash[j*4+i+2] & 0xff)<<8
                   | (hash[j*4+i+1] & 0xff)<<16
                   | (hash[j*4+i+0] & 0xff)<<24	);
//        cout<<r[j]<<endl;
    }
//    cout<<endl;
	return r;
}


	int 				itemtype;		// 1 string 2 integer 3 list 4 dictionary
	int					depth;
	string				s;
	long long int		i;
	vector<btitem>		l;
	map<string,btitem>	m;
		
		
};
	#define printtorrent 1
	#define debug 1
	#define vbv 1
class torrent{
	public:

	torrent(string &tor, string &dir){
		torrent_filename=tor;
		contents_directory=dir;
	}
	torrent(int c, const char*& a, const char*&b){
		if (debug) cout<<c<<"openconstructor\n";
		if (c>=2) torrent_filename=a; else return;
		if (debug) cout<<"fn assigned\n";
		if (c>=3) contents_directory=b;
		else contents_directory=path(torrent_filename);
		if (debug) cout<<"dir assigned\n";
		if (debug) cout<<torrent_filename<<endl<<contents_directory<<endl<<"endconstructor";
	}
	long long int get_combined_size(){
		//if(d.m.count["info"] and d.m["info"].m["files"])
		long long int r=0;
		if (debug) cout<<d.m["info"].m["files"].l.size()<<endl;
		for (int i=0;i<d.m["info"].m["files"].l.size();i++)
			r+= d.m["info"].m["files"].l[i].m["length"].i;
		return r;
	}
	string checksum_number(int index){
		char hex[]="0123456789abcdef";
		string return_string;
		for (int i=index*20;i<index*20+20;i++){
			return_string.append(1,hex[(d.m["info"].m["pieces"].s[i] & 0xf0)>>4]);
			return_string.append(1,hex[(d.m["info"].m["pieces"].s[i] & 15)]);	
		}
		return return_string;
	}
	
	int hashpiece(std::ifstream &is, int hashindex){
		SHA1 sha;
		int sentinel=sha.update_hashpiece(is, piecelength, fileindex, filenamelist);
		if (sentinel) return sentinel;
		if (sha.final()==checksum_number(hashindex))
			return 0;
		return 1;
	}
	string loadpiecefromstream(std::ifstream &is){
		string r,small;
		#define read_size_exponent 15
		int read_size=1<<read_size_exponent;
		#define read_size 64
//		#define fbuffer_size 64
//		#define BLOCK_BYTES 64
//		int gcount=0;
		#define printswitchfile 0
		int prevgcount=0;
		while (r.size()<piecelength and fileindex<filenamelist.size()){
			char buffer[read_size];
			is.read(buffer,read_size);
			r.append(buffer,is.gcount());
			prevgcount=is.gcount();
			while(prevgcount<read_size){
				is.close();				
				if (++fileindex==filenamelist.size()) break;
				is.open(filenamelist[fileindex], std::ios::binary);
				if(!is.is_open()) return "";
				if (printswitchfile) cout<<"switching to next file\n"<<filenamelist[fileindex]<<endl;
				prevgcount=is.gcount();
				is.read(buffer, read_size - is.gcount());
				r.append(buffer, is.gcount());
				prevgcount+=is.gcount();
			}

		}
	return r;
}
	int hashpiecestring(std::ifstream &is, int hashindex){
		string piece=loadpiecefromstream(is);
		assert(piece.size()==piecelength or fileindex==filenamelist.size());
		SHA1 sha;
		sha.update(piece);
		if (sha.final()==checksum_number(hashindex))
			return 0;
		return 1;
	}
	int load_stream_and_filelist(std::ifstream &stream){
		// contents in contents_directory
		// works for single file torrents because I coded that case in get_filenamelist()
		filenamelist = d.get_filenamelist(contents_directory);
		stream.open(filenamelist[0], std::ios::binary);
		if (stream.is_open()) return 0;
				
		// next try a subdirectory with the torrent name
		filenamelist.clear();
		if (d.m.count("info") and d.m["info"].m.count("name") ) 
			contents_directory = path(torrent_filename)+"\\"+d.m["info"].m["name"].s;
		filenamelist = d.get_filenamelist(contents_directory);
		stream.open(filenamelist[0], std::ios::binary);
		if (stream.is_open()) return 0;
		
		//	backup routine to directly input single file torrent
		filenamelist.clear();
		filenamelist.push_back(contents_directory);
		if (debug){printvector(filenamelist);
			cout<<"filenamelistsize="<<filenamelist.size()<<endl<<"contents_directory= "<<contents_directory<<endl;}
		stream.open(contents_directory, std::ios::binary);
		if (stream.is_open()) return 0;
			
		return 1;			
	}
	int verify_torrent(){
		if (debug){
			cout<<"torrent_filename="<<torrent_filename<<endl<<"contents_directory="<<contents_directory<<endl;
		}
		int r;
		r = d.decode_item_from_file(torrent_filename);
		if (r) {cout<<"d.decode_item_from_file() failed\nThat means source is invalid bencoded file\n"; return r;}
		else cout<<"Torrent sucessfully decoded.\n";
		if (printtorrent) d.print();
		
		ifstream stream;
		r=load_stream_and_filelist(stream);
		if (debug) cout<<"load_stream_and_filelist(stream) returned:"<<r<<"\nfilenamelist.size()="<<filenamelist.size()<<endl;
		if (r) return r;
		if (vbv) {
			vector<string> nopath=d.get_filenamelist("");
			cout<<"begin filenamelist\n";
			printvectorskipslash(nopath);
//			printvector(filenamelist);
			cout<<"end filenamelist\n\n";}
		if (vbv) cout<<"combined size="<<get_combined_size();
		piecelength = d.get_piecelength();
		if (vbv) cout<<" piecelength= "<<piecelength<<endl;
		numberofpieces = d.get_numberofpieces();
		if (vbv) {cout<<"numberofpieces= "<<numberofpieces<<endl<<"filenamelist[0]="<<filenamelist[0]<<"\nhashing";}
		time_t starttime=time(0);
		fileindex=0;
		int counter=0;
		for (int pieceindex=0;pieceindex<numberofpieces;pieceindex++){
			if (debug) cout<<".";
			if ( hashpiecestring(stream, pieceindex) == 0 ) counter++;
			else break;
		}
		cout<<"counter= "<<counter<<endl;
		if (counter==numberofpieces){
			time_t stoptime=time(0);
//			double elapsed=difftime(stoptime,starttime);
			double elapsed=stoptime-starttime;
			cout<<"\ntorrent verified\n";
			cout<<setprecision(3)<<"difftime(stoptime,starttime)="<<elapsed<<"\n"<<numberofpieces*piecelength/1000000/elapsed<<" MB/s\n";
			return 1;}
		else{
			cout<<"\ntorrent verification failed";
			return 0;}
	}
		string torrent_filename;
		string contents_directory;
		btitem d;
		int piecelength;
		int numberofpieces;
		int fileindex;		
		vector<string> filenamelist;
//		std::ifstream is;
};

void printhelp(){
	cout<<"\nTorrent hash check\n";
	cout<<"first argument: torrentfile\n";
	cout<<"second argument: directory containing files or filename of single file \n";}

int main(int argc, const char *argv[]){
	cout<<"argc="<<argc<<endl;
	if (argc<2) {printhelp(); return 0;}
	torrent t(argc, argv[1], argv[2]);
	cout<<"constructor finished\n";
	t.verify_torrent();
	char c;
	if (debug) cout<<"Program ran correctly. Press enter to exit.\n";
	cin.get(c);
	return 0;
}

