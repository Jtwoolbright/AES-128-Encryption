
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Maps.h"

using namespace std;

void get_key_from_user(const string s_key, unsigned char* key){
    unsigned int x1, x2;
    stringstream ss;
    int j = 0;
    for (int i = 0; i < 32; i = i + 2){
        ss << hex << s_key[i];
        ss >> x1;
        ss.clear();
        ss << hex << s_key[i + 1];
        ss >> x2;
        ss.clear();
        key[j] = (x1 * 16) + x2;
        j++;
    }
}

void get_key_schedule(unsigned char* key, unsigned char* key_schedule){
    unsigned char word[32];
    unsigned char temp[4];
    for (int i = 0; i <= 15; i++){
        word[i] = key[i];
        key_schedule[i] = key[i];
    }
    int i = 16;
    while (i < 176){
        temp[0] = word[15];
        temp[1] = word[14];
        temp[2] = word[13];
        temp[3] = word[12];
        temp[0] = sbox[temp[0]];
        temp[0] = temp[0] ^ rcon[i / 16];
        temp[1] = sbox[temp[1]];
        temp[2] = sbox[temp[2]];
        temp[3] = sbox[temp[3]];
        word[16] = word[0] ^ temp[0];
        word[17] = word[1] ^ temp[1];
        word[18] = word[2] ^ temp[2];
        word[19] = word[3] ^ temp[3];
        for (int j = 20; j < 32; j++) {
            word[j] = word[j - 16] ^ word[j - 4];
        }
        for (int k = 0; k < 16; k++){
            word[k] = word[k + 16];
            key_schedule[i] = word[k];
            i++;
        }
    }
}

void key_add(unsigned char* key_schedule, unsigned char* f_buffer, int round){
    int j = round * 16;
    for (int i = 0; i < 16; i++) {
		f_buffer[i] ^= key_schedule[j];
		j++;
	}
}

void sub_bytes(unsigned char* f_buffer){
    for (int i = 0; i < 16; i++) {
		f_buffer[i] = sbox[f_buffer[i]];
	}
}

void shift_rows(unsigned char* f_buffer){
    unsigned char temp[16];

	temp[0] = f_buffer[0];
	temp[1] = f_buffer[5];
	temp[2] = f_buffer[10];
	temp[3] = f_buffer[15];

	temp[4] = f_buffer[4];
	temp[5] = f_buffer[9];
	temp[6] = f_buffer[14];
	temp[7] = f_buffer[3];

	temp[8] = f_buffer[8];
	temp[9] = f_buffer[13];
	temp[10] = f_buffer[2];
	temp[11] = f_buffer[7];

	temp[12] = f_buffer[12];
	temp[13] = f_buffer[1];
	temp[14] = f_buffer[6];
	temp[15] = f_buffer[11];

	for (int i = 0; i < 16; i++) {
		f_buffer[i] = temp[i];
	}
}

void mix_columns(unsigned char* f_buffer){

    unsigned char temp[16];

	temp[0] = (unsigned char) multiply2[f_buffer[0]] ^ multiply3[f_buffer[1]] ^ f_buffer[2] ^ f_buffer[3];
	temp[1] = (unsigned char) f_buffer[0] ^ multiply2[f_buffer[1]] ^ multiply3[f_buffer[2]] ^ f_buffer[3];
	temp[2] = (unsigned char) f_buffer[0] ^ f_buffer[1] ^ multiply2[f_buffer[2]] ^ multiply3[f_buffer[3]];
	temp[3] = (unsigned char) multiply3[f_buffer[0]] ^ f_buffer[1] ^ f_buffer[2] ^ multiply2[f_buffer[3]];

	temp[4] = (unsigned char) multiply2[f_buffer[4]] ^ multiply3[f_buffer[5]] ^ f_buffer[6] ^ f_buffer[7];
	temp[5] = (unsigned char) f_buffer[4] ^ multiply2[f_buffer[5]] ^ multiply3[f_buffer[6]] ^ f_buffer[7];
	temp[6] = (unsigned char) f_buffer[4] ^ f_buffer[5] ^ multiply2[f_buffer[6]] ^ multiply3[f_buffer[7]];
	temp[7] = (unsigned char) multiply3[f_buffer[4]] ^ f_buffer[5] ^ f_buffer[6] ^ multiply2[f_buffer[7]];

	temp[8] = (unsigned char) multiply2[f_buffer[8]] ^ multiply3[f_buffer[9]] ^ f_buffer[10] ^ f_buffer[11];
	temp[9] = (unsigned char) f_buffer[8] ^ multiply2[f_buffer[9]] ^ multiply3[f_buffer[10]] ^ f_buffer[11];
	temp[10] = (unsigned char) f_buffer[8] ^ f_buffer[9] ^ multiply2[f_buffer[10]] ^ multiply3[f_buffer[11]];
	temp[11] = (unsigned char) multiply3[f_buffer[8]] ^ f_buffer[9] ^ f_buffer[10] ^ multiply2[f_buffer[11]];

	temp[12] = (unsigned char) multiply2[f_buffer[12]] ^ multiply3[f_buffer[13]] ^ f_buffer[14] ^ f_buffer[15];
	temp[13] = (unsigned char) f_buffer[12] ^ multiply2[f_buffer[13]] ^ multiply3[f_buffer[14]] ^ f_buffer[15];
	temp[14] = (unsigned char) f_buffer[12] ^ f_buffer[13] ^ multiply2[f_buffer[14]] ^ multiply3[f_buffer[15]];
	temp[15] = (unsigned char) multiply3[f_buffer[12]] ^ f_buffer[13] ^ f_buffer[14] ^ multiply2[f_buffer[15]];

	for (int i = 0; i < 16; i++) {
		f_buffer[i] = temp[i];
	}
}

int main(int argc, char* argv[]){

    if (argc > 1){

        string src_filepath = argv[1];
        string dst_filepath = src_filepath.substr(0, src_filepath.find_last_of('.')) + ".enc";
        ifstream src_file_descriptor;
        ofstream dst_file_descriptor;
        src_file_descriptor.open(src_filepath.c_str());
        dst_file_descriptor.open(dst_filepath.c_str());

        if (src_file_descriptor.fail()){
            cout << "File Not Found" << endl;
            return -1;
        }
        if (src_file_descriptor.eof()){
            cout << "File Is Empty" << endl;
            return -1;
        }

        cout << "Enter encryption key (Do not enter 0x before hex digits, Do not "
             << "include spaces)" << endl;
        string s_key;
        cin >> s_key;

        unsigned char key[16];
        get_key_from_user(s_key, key);

        unsigned char key_schedule[176];
        get_key_schedule(key, key_schedule);

        unsigned char f_buffer[16];

        while (!src_file_descriptor.eof()){

            src_file_descriptor.read((char*)f_buffer, 16);
            streamsize bytes_read = src_file_descriptor.gcount();

            if (bytes_read < 16){
                while (bytes_read < 16){
                    f_buffer[bytes_read] = 0;
                    bytes_read++;
                }
            }

            int round = 0;
            key_add(key_schedule, f_buffer, round);
            round++;

            for (int i = 1; i <= 9; i++){
                sub_bytes(f_buffer);
                shift_rows(f_buffer);
                mix_columns(f_buffer);
                key_add(key_schedule, f_buffer, round);
                round++;
            }

            sub_bytes(f_buffer);
            shift_rows(f_buffer);
            key_add(key_schedule, f_buffer, round);

            dst_file_descriptor.write((char*)f_buffer, 16);
        }

        src_file_descriptor.close();
        dst_file_descriptor.close();
    }
    else{
        cout << "Enter file to be encrypted into command line" << endl;
    }
    return 0;
}
