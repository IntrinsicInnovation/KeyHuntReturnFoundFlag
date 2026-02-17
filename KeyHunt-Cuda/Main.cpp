#include "Timer.h"
#include "KeyHunt.h"
#include "Base58.h"
#include "CmdParse.h"
#include <fstream>
#include <string>
#include <string.h>
#include <stdexcept>
#include <cassert>
#include <algorithm>

//khclient:

/*
#include <iostream>

#include <thread>
#include <chrono>
#include <future>
#include <cstdlib>
#include <curl/curl.h>
#include "json.hpp"
#include <filesystem>
#include <unistd.h>     // for readlink
#include <limits.h>     // for PATH_MAX
#include <inttypes.h>

*/


#ifndef WIN64
#include <signal.h>
#include <unistd.h>
#endif

#define RELEASE "1.07"

using namespace std;
bool should_exit = false;




//khclient additions below":


//using json = nlohmann::json;
//namespace fs = std::filesystem;

/*

static const std::string SERVER = "https://api.cocheat.com";
static const std::string WORKER_ID = "linux-integrated-1";

static size_t WriteCallback(void* contents,
	size_t size,
	size_t nmemb,
	void* userp) {
	((std::string*)userp)->append(
		(char*)contents, size * nmemb);
	return size * nmemb;
}
*/

/*

json httpPost(const std::string& url,
	const json& body) {

	CURL* curl = curl_easy_init();
	std::string response;

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(
		headers,
		"Content-Type: application/json");

	std::string payload = body.dump();

	curl_easy_setopt(curl, CURLOPT_URL,
		url.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER,
		headers);

	curl_easy_setopt(curl,
		CURLOPT_POST, 1L);

	curl_easy_setopt(curl,
		CURLOPT_POSTFIELDS,
		payload.c_str());

	curl_easy_setopt(curl,
		CURLOPT_POSTFIELDSIZE,
		payload.size());

	curl_easy_setopt(curl,
		CURLOPT_WRITEFUNCTION,
		WriteCallback);

	curl_easy_setopt(curl,
		CURLOPT_WRITEDATA,
		&response);

	curl_easy_setopt(curl,
		CURLOPT_SSL_VERIFYPEER, 0L);

	curl_easy_setopt(curl,
		CURLOPT_SSL_VERIFYHOST, 0L);

	curl_easy_setopt(curl,
		CURLOPT_SSLVERSION,
		CURL_SSLVERSION_TLSv1_2);

	CURLcode res =
		curl_easy_perform(curl);

	if (res != CURLE_OK) {
		std::cout << "CURL ERROR: "
			<< curl_easy_strerror(res)
			<< "\n";
	}

	long http_code = 0;
	curl_easy_getinfo(curl,
		CURLINFO_RESPONSE_CODE,
		&http_code);

	std::cout << "HTTP CODE: "
		<< http_code << "\n";

	std::cout << "SERVER RAW:\n"
		<< response << "\n";

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (response.empty())
		return json();

	return json::parse(response);
}





void checkFoundFile()
{
	char exePath[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
	if (len == -1) {
		perror("readlink");
		return;
	}
	exePath[len] = '\0';

	fs::path exeDir = fs::path(exePath).parent_path();
	fs::path file = exeDir / "foundNEW.txt";

	if (fs::exists(file))
	{
		std::cerr << "\n[!] PRIVATE KEY FOUND!\n"
			<< "[!] Check foundNEW.txt\n"
			<< "[!] File path: " << file << std::endl;

		exit(0);
	}
}

*/




/*
// ---------------- Leasing ----------------

json leaseChunk() {
	return httpPost(SERVER + "/lease", {
		{"workerId", WORKER_ID},
		{"count", 1}
		});
}

*/







// ----------------------------------------------------------------------------
void usage()
{
	printf("KeyHunt-Cuda [OPTIONS...] [TARGETS]\n");
	printf("Where TARGETS is one address/xpont, or multiple hashes/xpoints file\n\n");

	printf("-h, --help                               : Display this message\n");
	printf("-c, --check                              : Check the working of the codes\n");
	printf("-u, --uncomp                             : Search uncompressed points\n");
	printf("-b, --both                               : Search both uncompressed or compressed points\n");
	printf("-g, --gpu                                : Enable GPU calculation\n");
	printf("--gpui GPU ids: 0,1,...                  : List of GPU(s) to use, default is 0\n");
	printf("--gpux GPU gridsize: g0x,g0y,g1x,g1y,... : Specify GPU(s) kernel gridsize, default is 8*(Device MP count),128\n");
	printf("-t, --thread N                           : Specify number of CPU thread, default is number of core\n");
	printf("-i, --in FILE                            : Read rmd160 hashes or xpoints from FILE, should be in binary format with sorted\n");
	printf("-o, --out FILE                           : Write keys to FILE, default: Found.txt\n");
	printf("-m, --mode MODE                          : Specify search mode where MODE is\n");
	printf("                                               ADDRESS  : for single address\n");
	printf("                                               ADDRESSES: for multiple hashes/addresses\n");
	printf("                                               XPOINT   : for single xpoint\n");
	printf("                                               XPOINTS  : for multiple xpoints\n");
	printf("--coin BTC/ETH                           : Specify Coin name to search\n");
	printf("                                               BTC: available mode :-\n");
	printf("                                                   ADDRESS, ADDRESSES, XPOINT, XPOINTS\n");
	printf("                                               ETH: available mode :-\n");
	printf("                                                   ADDRESS, ADDRESSES\n");
	printf("-l, --list                               : List cuda enabled devices\n");
	printf("--range KEYSPACE                         : Specify the range:\n");
	printf("                                               START:END\n");
	printf("                                               START:+COUNT\n");
	printf("                                               START\n");
	printf("                                               :END\n");
	printf("                                               :+COUNT\n");
	printf("                                               Where START, END, COUNT are in hex format\n");
	printf("-r, --rkey Rkey                          : Random key interval in MegaKeys, default is disabled\n");
	printf("-v, --version                            : Show version\n");
}

// ----------------------------------------------------------------------------

void getInts(string name, vector<int>& tokens, const string& text, char sep)
{

	size_t start = 0, end = 0;
	tokens.clear();
	int item;

	try {

		while ((end = text.find(sep, start)) != string::npos) {
			item = std::stoi(text.substr(start, end - start));
			tokens.push_back(item);
			start = end + 1;
		}

		item = std::stoi(text.substr(start));
		tokens.push_back(item);

	}
	catch (std::invalid_argument&) {

		printf("Invalid %s argument, number expected\n", name.c_str());
		usage();
		exit(-1);

	}

}

// ----------------------------------------------------------------------------

int parseSearchMode(const std::string& s)
{
	std::string stype = s;
	std::transform(stype.begin(), stype.end(), stype.begin(), ::tolower);

	if (stype == "address") {
		return SEARCH_MODE_SA;
	}

	if (stype == "xpoint") {
		return SEARCH_MODE_SX;
	}

	if (stype == "addresses") {
		return SEARCH_MODE_MA;
	}

	if (stype == "xpoints") {
		return SEARCH_MODE_MX;
	}

	printf("Invalid search mode format: %s", stype.c_str());
	usage();
	exit(-1);
}

// ----------------------------------------------------------------------------

int parseCoinType(const std::string& s)
{
	std::string stype = s;
	std::transform(stype.begin(), stype.end(), stype.begin(), ::tolower);

	if (stype == "btc") {
		return COIN_BTC;
	}

	if (stype == "eth") {
		return COIN_ETH;
	}

	printf("Invalid coin name: %s", stype.c_str());
	usage();
	exit(-1);
}

// ----------------------------------------------------------------------------

bool parseRange(const std::string& s, Int& start, Int& end)
{
	size_t pos = s.find(':');

	if (pos == std::string::npos) {
		start.SetBase16(s.c_str());
		end.Set(&start);
		end.Add(0xFFFFFFFFFFFFULL);
	}
	else {
		std::string left = s.substr(0, pos);

		if (left.length() == 0) {
			start.SetInt32(1);
		}
		else {
			start.SetBase16(left.c_str());
		}

		std::string right = s.substr(pos + 1);

		if (right[0] == '+') {
			Int t;
			t.SetBase16(right.substr(1).c_str());
			end.Set(&start);
			end.Add(&t);
		}
		else {
			end.SetBase16(right.c_str());
		}
	}

	return true;
}

#ifdef WIN64
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType) {
	case CTRL_C_EVENT:
		//printf("\n\nCtrl-C event\n\n");
		should_exit = true;
		return TRUE;

	default:
		return TRUE;
	}
}
#else
void CtrlHandler(int signum) {
	printf("\n\nBYE\n");
	exit(signum);
}
#endif





int main(int argc, char** argv)
{

	/*curl_version_info_data* data =
		curl_version_info(CURLVERSION_NOW);

	if (data->features & CURL_VERSION_THREADSAFE) {
		printf("libcurl is thread-safe\n");
	}
	else {
		printf("libcurl is NOT thread-safe\n");
	}


	CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (res != CURLE_OK) {
		fprintf(stderr, "curl_global_init() failed: %s\n",
			curl_easy_strerror(res));
		return 1;
	}
	
	*/

	/*
	std::cout << "Total number of arguments: " << argc << std::endl;

	for (int i = 0; i < argc; ++i) {
		std::cout << "Argument " << i << ": " << argv[i] << std::endl;
	}
	*/


	
	

//	std::cout << "Integrated client started\n";



//	while (true) {


		/*

		json leaseResp = leaseChunk();

		if (!leaseResp.is_array() ||

			leaseResp.empty()) {

			std::cout << "No jobs, retrying...\n";

			std::this_thread::sleep_for(
				std::chrono::seconds(2));

			
			continue;
		}

		auto chunk = leaseResp[0];

		int chunkId = chunk["chunkId"];
		std::string start = chunk["start"];
		std::string end = chunk["end"];

		

		// --- STRIP 0x PREFIX ---
		if (start.rfind("0x", 0) == 0 || start.rfind("0X", 0) == 0)
			start = start.substr(2);

		if (end.rfind("0x", 0) == 0 || end.rfind("0X", 0) == 0)
			end = end.substr(2);

		


		std::cout << "\nLeased chunk "
			<< chunkId
			<< " | " << start
			<< " → " << end
			<< "\n";

			*/
		
//code to setup parameters from web service.  not used now
	/*
		std::vector<std::string> newargv = {
			"keyhunt",
			"-g",
			"--gpui", "0",
			"--gpux", "256,256",
			"-o", "foundNEW.txt",
			"-m", "address",
			"--coin", "BTC",
			//"--range", start + ":" + end,
			"--range", "72DCA255555556C3A3:72DCA25555614285A2",
			"1PWo3JeB9jrGwfHDNpdGK54CRas7fsVzXU"
		};
			
		char** c_args = new char* [newargv.size() + 1];
		for (size_t i = 0; i < newargv.size(); ++i) {
			c_args[i] = new char[newargv[i].size() + 1];
			std::strcpy(c_args[i], newargv[i].c_str());
		}
		c_args[newargv.size()] = nullptr;

		std::cout << "Total number of arguments(from web service): " << newargv.size() << std::endl;

		for (int i = 0; i < newargv.size(); ++i) {
			std::cout << "Argument " << i << ": " << c_args[i] << std::endl;
		}
		*/


		//khclient code ABOVE
		//basically loop and within the loop read the web service for the next chunk and setup argc and argv to parse as normal
		//below:






	// Global Init
	Timer::Init();
	rseed(Timer::getSeed32());

	bool gpuEnable = false;
	bool gpuAutoGrid = true;
	int compMode = SEARCH_COMPRESSED;
	vector<int> gpuId = { 0 };
	vector<int> gridSize;

	string outputFile = "Found.txt";

	string inputFile = "";	// for both multiple hash160s and x points
	string address = "";	// for single address mode
	string xpoint = "";		// for single x point mode

	std::vector<unsigned char> hashORxpoint;
	bool singleAddress = false;
	int nbCPUThread = Timer::getCoreNumber();

	bool tSpecified = false;
	bool useSSE = true;
	uint32_t maxFound = 1024 * 64;

	uint64_t rKey = 0;

	Int rangeStart;
	Int rangeEnd;
	rangeStart.SetInt32(0);
	rangeEnd.SetInt32(0);

	int searchMode = 0;
	int coinType = COIN_BTC;

	hashORxpoint.clear();

	// cmd args parsing
	CmdParse parser;
	parser.add("-h", "--help", false);
	parser.add("-c", "--check", false);
	parser.add("-l", "--list", false);
	parser.add("-u", "--uncomp", false);
	parser.add("-b", "--both", false);
	parser.add("-g", "--gpu", false);
	parser.add("", "--gpui", true);
	parser.add("", "--gpux", true);
	parser.add("-t", "--thread", true);
	parser.add("-i", "--in", true);
	parser.add("-o", "--out", true);
	parser.add("-m", "--mode", true);
	parser.add("", "--coin", true);
	parser.add("", "--range", true);
	parser.add("-r", "--rkey", true);
	parser.add("-v", "--version", false);

	if (argc == 1) {


	//printf("argc: %d", argc);
	//	if (newargv.size() == 1) {

		

		usage();
		return 0;
	}
	try {
		parser.parse(argc, argv);
		
		//parser.parse(newargv.size(), c_args);
		
		//replaced argv with c_args as it's read from the webservice.

	}
	catch (std::string err) {
		printf("Error: %s\n", err.c_str());
		usage();
		exit(-1);
	}
	std::vector<OptArg> args = parser.getArgs();

	for (unsigned int i = 0; i < args.size(); i++) {
		OptArg optArg = args[i];
		std::string opt = args[i].option;

		try {
			if (optArg.equals("-h", "--help")) {
				usage();
				return 0;
			}
			else if (optArg.equals("-c", "--check")) {
				printf("KeyHunt-Cuda v" RELEASE "\n\n");
				printf("\nChecking... Secp256K1\n\n");
				Secp256K1* secp = new Secp256K1();
				secp->Init();
				secp->Check();
				printf("\n\nChecking... Int\n\n");
				Int* K = new Int();
				K->SetBase16("3EF7CEF65557B61DC4FF2313D0049C584017659A32B002C105D04A19DA52CB47");
				K->Check();
				delete secp;
				delete K;
				printf("\n\nChecked successfully\n\n");
				return 0;
			}
			else if (optArg.equals("-l", "--list")) {
#ifdef WIN64
				GPUEngine::PrintCudaInfo();
#else
				printf("GPU code not compiled, use -DWITHGPU when compiling.\n");
#endif
				return 0;
			}
			else if (optArg.equals("-u", "--uncomp")) {
				compMode = SEARCH_UNCOMPRESSED;
			}
			else if (optArg.equals("-b", "--both")) {
				compMode = SEARCH_BOTH;
			}
			else if (optArg.equals("-g", "--gpu")) {
				gpuEnable = true;
				nbCPUThread = 0;
			}
			else if (optArg.equals("", "--gpui")) {
				string ids = optArg.arg;
				getInts("--gpui", gpuId, ids, ',');
			}
			else if (optArg.equals("", "--gpux")) {
				string grids = optArg.arg;
				getInts("--gpux", gridSize, grids, ',');
				gpuAutoGrid = false;
			}
			else if (optArg.equals("-t", "--thread")) {
				nbCPUThread = std::stoi(optArg.arg);
				tSpecified = true;
			}
			else if (optArg.equals("-i", "--in")) {
				inputFile = optArg.arg;
			}
			else if (optArg.equals("-o", "--out")) {
				outputFile = optArg.arg;
			}
			else if (optArg.equals("-m", "--mode")) {
				searchMode = parseSearchMode(optArg.arg);
			}
			else if (optArg.equals("", "--coin")) {
				coinType = parseCoinType(optArg.arg);
			}
			else if (optArg.equals("", "--range")) {
				std::string range = optArg.arg;
				parseRange(range, rangeStart, rangeEnd);
			}
			else if (optArg.equals("-r", "--rkey")) {
				rKey = std::stoull(optArg.arg);
			}
			else if (optArg.equals("-v", "--version")) {
				printf("KeyHunt-Cuda v" RELEASE "\n");
				return 0;
			}
		}
		catch (std::string err) {
			printf("Error: %s\n", err.c_str());
			usage();
			return -1;
		}
	}

	// 
	if (coinType == COIN_ETH && (searchMode == SEARCH_MODE_SX || searchMode == SEARCH_MODE_MX/* || compMode == SEARCH_COMPRESSED*/)) {
		printf("Error: %s\n", "Wrong search or compress mode provided for ETH coin type");
		usage();
		return -1;
	}
	if (coinType == COIN_ETH) {
		compMode = SEARCH_UNCOMPRESSED;
		useSSE = false;
	}
	if (searchMode == (int)SEARCH_MODE_MX || searchMode == (int)SEARCH_MODE_SX)
		useSSE = false;


	// Parse operands
	std::vector<std::string> ops = parser.getOperands();

	if (ops.size() == 0) {
		// read from file
		if (inputFile.size() == 0) {
			printf("Error: %s\n", "Missing arguments");
			usage();
			return -1;
		}
		if (searchMode != SEARCH_MODE_MA && searchMode != SEARCH_MODE_MX) {
			printf("Error: %s\n", "Wrong search mode provided for multiple addresses or xpoints");
			usage();
			return -1;
		}
	}
	else {
		// read from cmdline
		if (ops.size() != 1) {
			printf("Error: %s\n", "Wrong args or more than one address or xpoint are provided, use inputFile for multiple addresses or xpoints");
			usage();
			return -1;
		}
		if (searchMode != SEARCH_MODE_SA && searchMode != SEARCH_MODE_SX) {
			printf("Error: %s\n", "Wrong search mode provided for single address or xpoint");
			usage();
			return -1;
		}


		switch (searchMode) {
		case (int)SEARCH_MODE_SA:
		{
			address = ops[0];
			if (coinType == COIN_BTC) {
				if (address.length() < 30 || address[0] != '1') {
					printf("Error: %s\n", "Invalid address, must have Bitcoin P2PKH address or Ethereum address");
					usage();
					return -1;
				}
				else {
					if (DecodeBase58(address, hashORxpoint)) {
						hashORxpoint.erase(hashORxpoint.begin() + 0);
						hashORxpoint.erase(hashORxpoint.begin() + 20, hashORxpoint.begin() + 24);
						assert(hashORxpoint.size() == 20);
					}
				}
			}
			else {
				if (address.length() != 42 || address[0] != '0' || address[1] != 'x') {
					printf("Error: %s\n", "Invalid Ethereum address");
					usage();
					return -1;
				}
				address.erase(0, 2);
				for (int i = 0; i < 40; i += 2) {
					uint8_t c = 0;
					for (size_t j = 0; j < 2; j++) {
						uint32_t c0 = (uint32_t)address[i + j];
						uint8_t c2 = (uint8_t)strtol((char*)&c0, NULL, 16);
						if (j == 0)
							c2 = c2 << 4;
						c |= c2;
					}
					hashORxpoint.push_back(c);
				}
				assert(hashORxpoint.size() == 20);
			}
		}
		break;
		case (int)SEARCH_MODE_SX:
		{
			unsigned char xpbytes[32];
			xpoint = ops[0];
			Int* xp = new Int();
			xp->SetBase16(xpoint.c_str());
			xp->Get32Bytes(xpbytes);
			for (int i = 0; i < 32; i++)
				hashORxpoint.push_back(xpbytes[i]);
			delete xp;
			if (hashORxpoint.size() != 32) {
				printf("Error: %s\n", "Invalid xpoint");
				usage();
				return -1;
			}
		}
		break;
		default:
			printf("Error: %s\n", "Invalid search mode for single address or xpoint");
			usage();
			return -1;
			break;
		}
	}

	if (gridSize.size() == 0) {
		for (int i = 0; i < gpuId.size(); i++) {
			gridSize.push_back(-1);
			gridSize.push_back(128);
		}
	}
	if (gridSize.size() != gpuId.size() * 2) {
		printf("Error: %s\n", "Invalid gridSize or gpuId argument, must have coherent size\n");
		usage();
		return -1;
	}

	if (rangeStart.GetBitLength() <= 0) {
		printf("Error: %s\n", "Invalid start range, provide start range at least, end range would be: start range + 0xFFFFFFFFFFFFULL\n");
		usage();
		return -1;
	}
	if (nbCPUThread > 0 && gpuEnable) {
		printf("Error: %s\n", "Invalid arguments, CPU and GPU, both can't be used together right now\n");
		usage();
		return -1;
	}

	// Let one CPU core free per gpu is gpu is enabled
	// It will avoid to hang the system
	if (!tSpecified && nbCPUThread > 1 && gpuEnable)
		nbCPUThread -= (int)gpuId.size();
	if (nbCPUThread < 0)
		nbCPUThread = 0;


	printf("\n");
	printf("KeyHunt-Cuda v" RELEASE "\n");
	printf("\n");
	if (coinType == COIN_BTC)
		printf("COMP MODE    : %s\n", compMode == SEARCH_COMPRESSED ? "COMPRESSED" : (compMode == SEARCH_UNCOMPRESSED ? "UNCOMPRESSED" : "COMPRESSED & UNCOMPRESSED"));
	printf("COIN TYPE    : %s\n", coinType == COIN_BTC ? "BITCOIN" : "ETHEREUM");
	printf("SEARCH MODE  : %s\n", searchMode == (int)SEARCH_MODE_MA ? "Multi Address" : (searchMode == (int)SEARCH_MODE_SA ? "Single Address" : (searchMode == (int)SEARCH_MODE_MX ? "Multi X Points" : "Single X Point")));
	printf("DEVICE       : %s\n", (gpuEnable && nbCPUThread > 0) ? "CPU & GPU" : ((!gpuEnable && nbCPUThread > 0) ? "CPU" : "GPU"));
	printf("CPU THREAD   : %d\n", nbCPUThread);
	if (gpuEnable) {
		printf("GPU IDS      : ");
		for (int i = 0; i < gpuId.size(); i++) {
			printf("%d", gpuId.at(i));
			if (i + 1 < gpuId.size())
				printf(", ");
		}
		printf("\n");
		printf("GPU GRIDSIZE : ");
		for (int i = 0; i < gridSize.size(); i++) {
			printf("%d", gridSize.at(i));
			if (i + 1 < gridSize.size()) {
				if ((i + 1) % 2 != 0) {
					printf("x");
				}
				else {
					printf(", ");
				}

			}
		}
		if (gpuAutoGrid)
			printf(" (Auto grid size)\n");
		else
			printf("\n");
	}

	

	printf("SSE          : %s\n", useSSE ? "YES" : "NO");
	//printf("RKEY         : %llu Mkeys\n", rKey);
	printf("RKEY         : %" PRIu64 " Mkeys\n", rKey);
	printf("MAX FOUND    : %d\n", maxFound);
	if (coinType == COIN_BTC) {
		switch (searchMode) {
		case (int)SEARCH_MODE_MA:
			printf("BTC HASH160s : %s\n", inputFile.c_str());
			break;
		case (int)SEARCH_MODE_SA:
			printf("BTC ADDRESS  : %s\n", address.c_str());
			break;
		case (int)SEARCH_MODE_MX:
			printf("BTC XPOINTS  : %s\n", inputFile.c_str());
			break;
		case (int)SEARCH_MODE_SX:
			printf("BTC XPOINT   : %s\n", xpoint.c_str());
			break;
		default:
			break;
		}
	}
	else {
		switch (searchMode) {
		case (int)SEARCH_MODE_MA:
			printf("ETH ADDRESSES: %s\n", inputFile.c_str());
			break;
		case (int)SEARCH_MODE_SA:
			printf("ETH ADDRESS  : 0x%s\n", address.c_str());
			break;
		default:
			break;
		}
	}
	printf("OUTPUT FILE  : %s\n", outputFile.c_str());


#ifdef WIN64
	if (SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
		KeyHunt* v;
		switch (searchMode) {
		case (int)SEARCH_MODE_MA:
		case (int)SEARCH_MODE_MX:
			v = new KeyHunt(inputFile, compMode, searchMode, coinType, gpuEnable, outputFile, useSSE,
				maxFound, rKey, rangeStart.GetBase16(), rangeEnd.GetBase16(), should_exit);
			break;
		case (int)SEARCH_MODE_SA:
		case (int)SEARCH_MODE_SX:
			v = new KeyHunt(hashORxpoint, compMode, searchMode, coinType, gpuEnable, outputFile, useSSE,
				maxFound, rKey, rangeStart.GetBase16(), rangeEnd.GetBase16(), should_exit);
			break;
		default:
			printf("\n\nNothing to do, exiting\n");
			return 0;
			break;
		}
		v->Search(nbCPUThread, gpuId, gridSize, should_exit);

		int found = v->getNbFoundKey();
		//add a flag to return found when a private key found

	//	printf("AFTER Search finished");
		printf("FOUND = %d", found);


		delete v;

		printf("\n\nBYE\n");
		curl_global_cleanup();
	//	return 0;
		return found;
	}
	else {
		printf("Error: could not set control-c handler\n");
		return -1;
	}
#else
	signal(SIGINT, CtrlHandler);
	KeyHunt* v;
	switch (searchMode) {
	case (int)SEARCH_MODE_MA:
	case (int)SEARCH_MODE_MX:
		v = new KeyHunt(inputFile, compMode, searchMode, coinType, gpuEnable, outputFile, useSSE,
			maxFound, rKey, rangeStart.GetBase16(), rangeEnd.GetBase16(), should_exit);
		break;
	case (int)SEARCH_MODE_SA:
	case (int)SEARCH_MODE_SX:
		v = new KeyHunt(hashORxpoint, compMode, searchMode, coinType, gpuEnable, outputFile, useSSE,
			maxFound, rKey, rangeStart.GetBase16(), rangeEnd.GetBase16(), should_exit);
		break;
	default:
		printf("\n\nNothing to do, exiting\n");
		return 0;
		break;
	}
	v->Search(nbCPUThread, gpuId, gridSize, should_exit);

	int found = v->getNbFoundKey();
	//add a flag to return found when a private key found

	delete v;
	//printf("win64 NOT defined, existing linux code");
	printf("FOUND = %d", found);
	//return 0;
	return found;
#endif


	//khclient code below


	// Don't forget to free the memory!
//	for (size_t i = 0; i < newargv.size(); ++i) delete[] c_args[i];
//	delete[] c_args;


//	std::cout << "KeyHunt finished \n";

	// Mark complete
	//CJ remove lease for testing
	/*httpPost(SERVER + "/complete", {
		{"workerId", WORKER_ID},
		{"chunkId", chunkId}
		});
		*/

	//checkFoundFile();

//	}//While Loop


	//return 0;

	

}


