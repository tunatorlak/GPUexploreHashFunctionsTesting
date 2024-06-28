#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <random>

#define NR_HASH_FUNCTIONS	32
#define MAX_HASH_FUNCTIONS	32
#define EMPTY_NODE					0xFFFFFFFFFFFFFFFF
// Error value to indicate a full global hash table.
#define HASHTABLE_FULL 				0xFFFFFFFF

constexpr uint32_t NR_XOR_CONSTANTS_PER_FUN = 6;
constexpr uint32_t NR_UHASH_CONSTANTS_PER_FUN = 2;
constexpr uint32_t XOR_CONSTANTS_TABLE_SIZE = MAX_HASH_FUNCTIONS * NR_XOR_CONSTANTS_PER_FUN;
constexpr uint32_t UHASH_CONSTANTS_TABLE_SIZE = MAX_HASH_FUNCTIONS * NR_UHASH_CONSTANTS_PER_FUN;
constexpr uint32_t NR_RHASH_CONSTANTS_PER_FUN = 2 * 2;
constexpr uint32_t RHASH_CONSTANTS_TABLE_SIZE = MAX_HASH_FUNCTIONS * NR_RHASH_CONSTANTS_PER_FUN;
constexpr uint32_t INIT_RHASH_CONSTANTS_TABLE_SIZE = 2 * 2;
constexpr uint8_t INTERNAL_BIT_WIDTH = 29;
constexpr uint8_t INTERNAL_BIT_WIDTH_X2 = 58;
constexpr uint64_t INTERNAL_HASH_MASK = 0x1fffffff;
constexpr uint64_t INTERNAL_HASH_MASK_X2 = 0x3ffffffffffffff;
constexpr uint64_t UHASH_INIT_0 = 0xD1B54A32D192ED03;
constexpr uint64_t UHASH_INIT_1 = 0xAEF17502108EF2D9;


typedef uint64_t 		nodetype;
// indices used for references to internal (non-root) tree nodes are restricted to 32 bits.
typedef uint32_t 		internal_indextype;
// type of global memory indices used.
typedef uint32_t 		indextype;

using namespace std;

const uint8_t XOR_CONSTANTS[XOR_CONSTANTS_TABLE_SIZE] = {
				27, 25, 51, 38, 19, 21,
				21, 40, 48, 19, 28, 13,
				14, 27, 50, 31, 47, 23,
				53, 31, 10, 34, 23, 28,
				19, 39, 49, 36, 11, 35,
				43, 45, 51, 36, 20, 33,
				50, 45, 51, 18, 52, 41,
				12, 39, 37, 14, 11, 52,
				30, 18, 26, 24, 40, 43,
				40, 17, 15, 16, 26, 45,
				18, 43, 22, 36, 17, 47,
				49, 32, 45, 15, 16, 53,
				54, 12, 19, 16, 26, 53,
				27, 11, 41, 38, 36, 46,
				55, 43, 28, 38, 45, 24,
				26, 21, 51, 35, 24, 34,
				30, 38, 21, 24, 20, 46,
				26, 46, 20, 12, 25, 28,
				52, 26, 49, 36, 27, 44,
				40, 20, 21, 27, 23, 54,
				25, 22, 27, 34, 41, 43,
				12, 52, 41, 36, 44, 46,
				40, 23, 21, 50, 20, 31,
				10, 18, 43, 11, 56, 22,
				11, 38, 54, 29, 23, 25,
				46, 22, 53, 42, 41, 14,
				53, 35, 20, 51, 29, 15,
				48, 56, 41, 35, 47, 55,
				37, 16, 20, 31, 38, 36,
				18, 31, 11, 55, 25, 50,
				10, 20, 37, 43, 44, 53,
				41, 50, 33, 45, 28, 20
};

const uint64_t UHASH_CONSTANTS[UHASH_CONSTANTS_TABLE_SIZE] = {
				0xd1b54a32d192e801, 	0xdb4f0b9175ae2001, 		// HASH CONSTANTS of FUNCTION 0
				0xe19b01aa9d42c801, 	0xe60e2b722b53a801, 		// HASH CONSTANTS of FUNCTION 1
				0xe95e1dd17d358001, 	0xebedeed9d803c001, 		// HASH CONSTANTS of FUNCTION 2
				0xedf84ed418562801, 	0xefa239aadff08001, 		// HASH CONSTANTS of FUNCTION 3
				0xf104272092f58001, 	0xf22eecf611d94001, 		// HASH CONSTANTS of FUNCTION 4
				0xf32e81f362a6a001, 	0xf40ba295557eb001, 		// HASH CONSTANTS of FUNCTION 5
				0xf4ccd627d640d001, 	0xf57716bc263d1001, 		// HASH CONSTANTS of FUNCTION 6
				0xf60e40931cdda001, 	0xf6955e0400b92001, 		// HASH CONSTANTS of FUNCTION 7
				0xf70edc6ab8a79801, 	0xf77cb1d641353801, 		// HASH CONSTANTS of FUNCTION 8
				0xf7e0785b8323f801, 	0xf83b8239bbcd4801, 		// HASH CONSTANTS of FUNCTION 9
				0xf88ee8ec947b7001, 	0xf8db9899f68de001, 		// HASH CONSTANTS of FUNCTION 10
				0xf92258d845901001, 	0xf963d37e4fa5d001, 		// HASH CONSTANTS of FUNCTION 11
				0xf9a099fb4affd801, 	0xf9d9299227c14801, 		// HASH CONSTANTS of FUNCTION 12
				0xfa0deebb02ba3801, 	0xfa3f47dc2ff8c801, 		// HASH CONSTANTS of FUNCTION 13
				0xfa6d877fe594b801, 	0xfa98f62288fc8801, 		// HASH CONSTANTS of FUNCTION 14
				0xfac1d3af0186d001, 	0xfae858b989a00801, 		// HASH CONSTANTS of FUNCTION 15
				0xfb0cb785ca5e7001, 	0xfb2f1ce245c55001, 		// HASH CONSTANTS of FUNCTION 16
				0xfb4fb0e0f5cae001, 	0xfb6e977365a09801, 		// HASH CONSTANTS of FUNCTION 17
				0xfb8bf0ef48515801, 	0xfba7da7f9468c001, 		// HASH CONSTANTS of FUNCTION 18
				0xfbc26e85670c2801, 	0xfbdbc4eb5518c801, 		// HASH CONSTANTS of FUNCTION 19
				0xfbf3f36d5503f801, 	0xfc0b0dd7093c7801, 		// HASH CONSTANTS of FUNCTION 20
				0xfc212639e3442801, 	0xfc364d1c57747001, 		// HASH CONSTANTS of FUNCTION 21
				0xfc4a91a32534d801, 	0xfc5e01b58cde5001, 		// HASH CONSTANTS of FUNCTION 22
				0xfc70aa1d29a5c001, 	0xfc8296a209365001, 		// HASH CONSTANTS of FUNCTION 23
				0xfc93d22382ee8801, 	0xfca466ae3d046801, 		// HASH CONSTANTS of FUNCTION 24
				0xfcb45d8fbd739801, 	0xfcc3bf67d6e62801, 		// HASH CONSTANTS of FUNCTION 25
				0xfcd29438364ab001, 	0xfce0e3724c1ff001, 		// HASH CONSTANTS of FUNCTION 26
				0xfceeb403c4530801, 	0xfcfc0c61b8a44001, 		// HASH CONSTANTS of FUNCTION 27
				0xfd08f292c3a4a001, 	0xfd156c38155d5001, 		// HASH CONSTANTS of FUNCTION 28
				0xfd217e95a66c3801, 	0xfd2d2e99a2b3f801, 		// HASH CONSTANTS of FUNCTION 29
				0xfd3880e3219a5801, 	0xfd4379c83f188801, 		// HASH CONSTANTS of FUNCTION 30
				0xfd4e1d5ba6896801, 	0xfd586f719e1bc001, 		// HASH CONSTANTS of FUNCTION 31
};

// XOR two times bit shift function for 64 bits.
uint64_t xor_shft2_64(const uint64_t& x, const uint8_t& a, const uint8_t& b) {
	uint64_t y = (x ^ (x << a));
	y ^= (x >> b);
	return y;
}

// Initial 64-bit hash function.
uint64_t UHASH_INIT(const nodetype& node) {
	uint64_t node1 = xor_shft2_64((uint64_t)node, XOR_CONSTANTS[0], (64 - XOR_CONSTANTS[0]));
	node1 ^= UHASH_INIT_0;
	node1 *= UHASH_INIT_1;
	return node1;
}

uint64_t HASH64(const uint16_t& id, const nodetype& node) {
	const uint16_t XOR_IDX = id * NR_XOR_CONSTANTS_PER_FUN;
	const uint16_t HASH_IDX = id * NR_UHASH_CONSTANTS_PER_FUN;
	//assert(XOR_IDX < XOR_CONSTANTS_TABLE_SIZE);
	//assert(HASH_IDX < UHASH_CONSTANTS_TABLE_SIZE);
	uint64_t node1 = node;
	node1 = xor_shft2_64(node1, XOR_CONSTANTS[XOR_IDX + 0], (64 - XOR_CONSTANTS[XOR_IDX + 0]));
	node1 = xor_shft2_64(node1, XOR_CONSTANTS[XOR_IDX + 1], (64 - XOR_CONSTANTS[XOR_IDX + 1]));
	node1 *= UHASH_CONSTANTS[HASH_IDX + 0];
	node1 ^= (node1 >> XOR_CONSTANTS[XOR_IDX + 2]);
	node1 ^= (node1 >> XOR_CONSTANTS[XOR_IDX + 3]);
	node1 ^= (node1 >> XOR_CONSTANTS[XOR_IDX + 4]);
	node1 *= UHASH_CONSTANTS[HASH_IDX + 1];
	node1 ^= (node1 >> XOR_CONSTANTS[XOR_IDX + 5]);
	return node1;
}

// Hash function for non-internal nodes.
nodetype UHASH(const uint16_t& id, const nodetype& node) {
	uint64_t node1 = HASH64(id, node);
	return node1;
}

// Extract a global memory internal hash table index from a given hash.
internal_indextype get_index_internal(nodetype h) {
	return (internal_indextype)(h & INTERNAL_HASH_MASK);
}

//everything above this is copied from GPUexplore

uint64_t INPUT_AMOUNT = 600000000; //number of inputs to be tested
const int HASH_TABLE_SIZE = 29; //2^n

uint64_t random_num() {
	random_device dev;
	mt19937 rng(dev());
	uniform_int_distribution<mt19937::result_type> dist(0, 75000000); // distribution in range [0, 75M]
	return dist(rng);
}

string toB(uint64_t number) { //converts to binary
	// input number
	long long int n = (long long int)(log2(number));

	// binary output
	// using the inbuilt function
	string result = bitset<64>(number).to_string().substr(64 - n- 1);

	while (result.size()<64) {
		result = "0" + result;
	}
	return result;
}

void print_result(vector<int> result, int input_amount) {
	for (int i = 0;i < result.size();i++) {
		cout << 100 * (double)result[i] / (double)input_amount << "% ";
	}
	cout << endl;
}

vector<int> compare(vector<uint64_t> ints, vector<int> result) {

	string inp = toB(ints[0]);
	string out = toB(ints[1]);

	for (int i = 0;i < inp.size();i++) {
		if (inp[i] == out[i]) {
			result[i]++;
		}
	}
	return result;
}

bool random_bool() {
	random_device dev;
	mt19937 rng(dev());
	uniform_int_distribution<mt19937::result_type> dist(0, 1); // distribution in range [0, 1]
	return dist(rng);
}

uint64_t random_bits(int index, bool num) { //random bitset

	bitset<64> bits;

	for (int j = 0;j < 64;j++) {
		if (random_bool()) {
			bits.flip(j);
		}
	}
	bits[index] = num;

	uint64_t result = bits.to_ullong();

	return result;
}

void collisiontest_normal() { //collision test with non-random inputs

	const nodetype node = 1;
	indextype addr = HASHTABLE_FULL;
	for (uint64_t rep = 0;rep < 10;rep = rep++) { //different tests

		cout << "Testing inputs starting from " << 1000000000 * rep << "." << endl;
		vector<bool> table(pow(2, HASH_TABLE_SIZE), false); //create hash table

		bool ht_full = false;
		for (uint64_t i = 0;i < INPUT_AMOUNT;i++) { //for every input

			if (ht_full) { //stop if hash table is full
				break;
			}

			nodetype e1 = UHASH_INIT(1000000000 * rep + i); //input is the number in paranthesis

			for (uint16_t j = 0;j < NR_HASH_FUNCTIONS;j++) { //for every hash function
				nodetype e2 = UHASH(j, e1);
				addr = get_index_internal(e2);
				if (table[addr] == false) { //if that address is empty
					table[addr] = true; //it is no longer empty
					break;
				}
				if (j == NR_HASH_FUNCTIONS - 1) { //failed to hash input
					cout << i - 1 << " inputs were hashed before collision." << endl;
					cout << 100 * ((i - 1) / pow(2, HASH_TABLE_SIZE)) << "% of the hash table is utilized." << endl << endl;
					ht_full = true;
				}
			}
		}
	}
}

void collisiontest_random() { //collision test with random inputs

	uint64_t input = 0;
	const nodetype node = 1;
	indextype addr = HASHTABLE_FULL;
	for (uint64_t rep = 0;rep < 10;rep = rep++) { //different tests

		cout << "Testing random inputs." << endl;
		vector<bool> table(pow(2, HASH_TABLE_SIZE), false); //create hash table

		bool ht_full = false;
		for (uint64_t i = 0;i < INPUT_AMOUNT;i++) { //for every input

			if (ht_full) { //stop if hash table is full
				break;
			}

			nodetype e1 = UHASH_INIT(input);
			input = input + 500000000 + random_num(); //input is being incremented randomly

			for (uint16_t j = 0;j < NR_HASH_FUNCTIONS;j++) { //for every hash function
				nodetype e2 = UHASH(j, e1);
				addr = get_index_internal(e2);
				if (table[addr] == false) { //if that address is empty
					table[addr] = true; //it is no longer empty
					break;
				}
				if (j == NR_HASH_FUNCTIONS - 1) { //failed to hash input
					cout << i - 1 << " inputs were hashed before collision." << endl;
					cout << 100 * ((i - 1) / pow(2, HASH_TABLE_SIZE)) << "% of the hash table is utilized." << endl << endl;
					ht_full = true;
				}
			}
		}
	}
}

void avalanche_random_each_function() { //avalanche test for each function individually

	uint64_t input = 0;
	vector<uint64_t> outputs(2,0);

	int input_amount = 1000;
	for (uint16_t j = 0;j < NR_HASH_FUNCTIONS;j++) { //for every hash function
		cout << "Testing hash function "<<j <<"."<< endl;
		for (int bit = 0;bit < 64;bit++) { //for every bit
			vector<int> result(64, 0);
			for (uint64_t i = 0;i < input_amount;i++) { //for every random input

				for (int num = 0;num < 2;num++) { //for 0 and 1
					
					input = random_bits(bit,num); //create input

					nodetype e1 = UHASH_INIT(input);
					nodetype e2 = UHASH(j, e1);
					outputs[num] = e2;

				}
				result = compare(outputs,result);
			}
			print_result(result,input_amount);
		}
		cout << endl;
	}
}

void avalanche_random() { //avalanche test for all functions combined

	uint64_t input = 0;
	vector<uint64_t> outputs(2, 0);

	int input_amount = 1000;
	for (int bit = 0;bit < 64;bit++) { //for every bit
		vector<int> result(64, 0);
		for (uint64_t i = 0;i < input_amount;i++) { //for every random input
			for (uint16_t j = 0;j < NR_HASH_FUNCTIONS;j++) { //for every hash function

				for (int num = 0;num < 2;num++) { //for 0 and 1

					input = random_bits(bit, num); //create input

					nodetype e1 = UHASH_INIT(input);
					nodetype e2 = UHASH(j, e1);
					outputs[num] = e2;

				}
				result = compare(outputs, result);
			}
		}
		print_result(result, input_amount*32);
	}
}

int main() { //main is only used to call the correct testing function

	collisiontest_normal();
	//collisiontest_random();
	//avalanche_random_each_function();
	//avalanche_random();

	return 0;
}