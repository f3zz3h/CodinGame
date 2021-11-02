#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <bitset>
#include <sstream>
#include <iomanip> //  setw && setfill


using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

const unsigned long round_constants[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

unsigned long h0;
unsigned long h1;
unsigned long h2;
unsigned long h3;
unsigned long h4;
unsigned long h5;
unsigned long h6;
unsigned long h7;

#define ROTATE_RIGHT(word,bits) (((word) >> (bits)) | ((word) << (32-(bits))))

vector<unsigned long> convert_to_binary(string message)
{
    vector<unsigned long> binary_msg;

    for (size_t i = 0; i < message.size(); ++i)
    {
        //Creates "temp" value to store the binary value
        bitset<8> temp (message.c_str()[i]);
        binary_msg.push_back(temp.to_ulong());
    }

    return binary_msg;
}

string ultoah(unsigned long long_in)
{
	bitset<32> long_bit(long_in);
	unsigned val = long_bit.to_ulong();

	stringstream sstream;
	sstream << hex << setw(8) << setfill('0') << val;
	string hex_val;
	sstream >> hex_val;
	return hex_val;
}

vector<unsigned long> pad_to_512(vector<unsigned long> message)
{
    int orig_length_in_bits = message.size() * 8;
    int length_in_bits = message.size() * 8;

    // Append a single 1 
    // 1000 0000
    message.push_back(0x80);

        // Pad with 0’s until data is a multiple of 512, less 64 bits (448 bits in our case):
    // len * 8 + 8 for single 1 and its 7 0's + 64 less bits
    cerr << orig_length_in_bits << endl;
    if ( orig_length_in_bits > 512 )
    {
        length_in_bits = orig_length_in_bits % 512;
    }
    cerr << length_in_bits << endl;

    int padding_required = 512 - (length_in_bits + 8 + 64);
    cerr << padding_required << endl;
    for(int i = 0; i < padding_required / 8; i++)
		message.push_back(0x00000000);

    // Now append the original length of message in remaining 64bits
    bitset<64> size_64_bit(orig_length_in_bits);
    
    string size_in_bytes = size_64_bit.to_string();

    // loop for number of bytes to fill
    for ( int i=0 ; i < 64; i+=8 )
    {
        //Grab a byte and push to back
        bitset<8> char_to_bitset (size_in_bytes.substr(i,8));
        message.push_back(char_to_bitset.to_ulong());
    }
    
    return message;
}

string create_message_schedule(vector<unsigned long> message)
{
    unsigned long w[64] = {0};

    // Loop through the 64 sections by 4 steps and merge those 4 sections.
	for(int i = 0; i < 64; i = i + 4)
	{
		// Lets make a big 32 bit section first.
		bitset<32> temp(0);

		// Shift the blocks to their assigned spots and OR them with the original
		// to combine them.
		temp = (unsigned long) message[i] << 24;
		temp |= (unsigned long) message[i + 1] << 16;
		temp |= (unsigned long) message[i + 2] << 8;
		temp |= (unsigned long) message[i + 3];

		// Puts the new 32 bit word into the correct output array location.
		w[i/4] = temp.to_ulong();
	}
    
    //For i from w[16…63]:
    //    s0 = (w[i-15] rightrotate 7) xor (w[i-15] rightrotate 18) xor (w[i-15] rightshift 3)
    //   s1 = (w[i- 2] rightrotate 17) xor (w[i- 2] rightrotate 19) xor (w[i- 2] rightshift 10) 
    // w[i] = w[i-16] + s0 + w[i-7] + s1
    for (int i = 16; i < 64; i++)
    {
        unsigned long s0 = ROTATE_RIGHT(w[i-15],7) ^ ROTATE_RIGHT(w[i-15] ,18) ^ (w[i-15] >> 3);
        unsigned long s1 = ROTATE_RIGHT(w[i- 2],17) ^ ROTATE_RIGHT(w[i- 2],19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1 ;
        w[i] = w[i] & 0xFFFFFFFF;
    }

    cerr << ultoah(h0) + ultoah(h1) +ultoah(h2) +ultoah(h3) +ultoah(h4) +ultoah(h5) +ultoah(h6) +ultoah(h7) << endl;
    // Initialize working variables to current hash value:
    unsigned long a = h0;
    unsigned long b = h1;
    unsigned long c = h2;
    unsigned long d = h3;
    unsigned long e = h4;
    unsigned long f = h5;
    unsigned long g = h6;
    unsigned long h = h7;

    /* Compression function main loop:
    for i from 0 to 63
        S1 := (e rightrotate 6) xor (e rightrotate 11) xor (e rightrotate 25)
        ch := (e and f) xor ((not e) and g)
        temp1 := h + S1 + ch + k[i] + w[i]
        S0 := (a rightrotate 2) xor (a rightrotate 13) xor (a rightrotate 22)
        maj := (a and b) xor (a and c) xor (b and c)
        temp2 := S0 + maj
 
        h := g
        g := f
        f := e
        e := d + temp1
        d := c
        c := b
        b := a
        a := temp1 + temp2
    */

    for (int i = 0; i < 64; i++)
    {
        unsigned long S1 = ROTATE_RIGHT(e, 6) ^ ROTATE_RIGHT(e, 11) ^  ROTATE_RIGHT(e, 25);
        unsigned long ch = (e & f) ^ (~(e) & g);
        unsigned long temp1 = h + S1 + ch + round_constants[i] + w[i];
        unsigned long S0 = ROTATE_RIGHT(a, 2) ^ ROTATE_RIGHT(a,13) ^ ROTATE_RIGHT(a,22);
        unsigned long maj = (a & b) ^ (a & c) ^ (b & c);
        unsigned long temp2 = S0 + maj;

        h = g;
        g = f;
        f = e;
        e = (d + temp1) & 0xFFFFFFFF;
		d = c;
		c = b;
		b = a;
		a = (temp1 + temp2) & 0xFFFFFFFF; 
    }

    //Add the compressed chunk to the current hash value:
    h0 = (h0 + a) & 0xFFFFFFFF;
    h1 = (h1 + b) & 0xFFFFFFFF;
    h2 = (h2 + c) & 0xFFFFFFFF;
    h3 = (h3 + d) & 0xFFFFFFFF;
    h4 = (h4 + e) & 0xFFFFFFFF;
    h5 = (h5 + f) & 0xFFFFFFFF;
    h6 = (h6 + g) & 0xFFFFFFFF;
    h7 = (h7 + h) & 0xFFFFFFFF;
    
    //Produce the final hash value (big-endian):
    //digest := hash := h0 append h1 append h2 append h3 append h4 append h5 append h6 append h7
    string digest = ultoah(h0) + ultoah(h1) +ultoah(h2) +ultoah(h3) +ultoah(h4) +ultoah(h5) +ultoah(h6) +ultoah(h7);

    return digest;
}

int main()
{
    string message;
    getline(cin, message);
    string encoded = "";
    cerr << message << " length " << message.length() << endl;
 
    h0 = 0xcbbb9d5d;
    h1 = 0x629a292a;
    h2 = 0x9159015a;
    h3 = 0x152fecd8;
    h4 = 0x67332667;
    h5 = 0x8eb44a87;
    h6 = 0xdb0c2e0d;
    h7 = 0x47b5481d;
   /* //Actual hash values
    h0 = 0x6a09e667;
	h1 = 0xbb67ae85;
	h2 = 0x3c6ef372;
	h3 = 0xa54ff53a;
	h4 = 0x510e527f;
	h5 = 0x9b05688c;
	h6 = 0x1f83d9ab;
	h7 = 0x5be0cd19;
*/

    cerr << ".." << message << ".." << message.length() << endl;
    vector<unsigned long> message_binary = convert_to_binary(message);
    vector<unsigned long> padded_msg = pad_to_512(message_binary);

    for (int i = 0; i < message.length(); i+=64)
    {
        vector<unsigned long> sub_vector (&padded_msg[i], &padded_msg[i+64] );
        for(int j = 0; j < 64; j++)
        {
        if (j % 4 == 0)
            cerr << endl;
            cerr << j << ": " << "0x" + ultoah(sub_vector[j]) << "    ";
        }
        cerr << endl;
        encoded = create_message_schedule(sub_vector);
        cerr << encoded << endl;
    }
    // Write an answer using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;
  
    cout << encoded << endl;
}
