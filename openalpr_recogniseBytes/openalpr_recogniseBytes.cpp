/* This project relies on the recognise bytes form of the alpr cloud api
and works through the following steps
1.	Image is read and coverted to binary format
2.	Image is then piped through ostringstream and converted to string
3.	The additional methods outside of the main code convert this string to base64
4.	This base64 encoded string is then sent to the cloud api

USE CTRL + F5 TO RUN THE PROJECT SO YOU CAN SEE THE CONSOLE OUTPUT

API DETAILS
web: http://doc.openalpr.com/api/cloudapi.html#/default

BASE64_encode from: (further reading)
https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp
*/

#include "stdafx.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>
#include <iostream>
#include <fstream>
#include <string>


using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace std;

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* buf, unsigned int bufLen) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (bufLen--) {
		char_array_3[i++] = *(buf++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i <4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';
	}

	return ret;
}


int main()
{
	string image_path = "D:\\uk-3.jpg";
	wstring country_code = L"eu";

	// Read Image
	std::ifstream image(image_path, std::ios::in | std::ios::binary);
	std::ostringstream oss;
	oss << image.rdbuf();
	std::string s(oss.str());

	// Convert to base64 format using two functions above
	std::string encoded = base64_encode(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());

	// image in base64 bytes -- Used an online tool to convert this one <-- not needed anymore but at https://www.base64-image.de/ if you're interested
	//string filePath = "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";

	wstring baseUrl = L"https://api.openalpr.com/v2/";
	http_client httpClient(baseUrl);
	uri_builder builder(baseUrl);

	// Now build your URL based on the service you wish to use
	builder.append_path(U("recognize_bytes"));
	builder.append_query(U("secret_key"), U("sk_DEMODEMODEMODEMODEMODEMO"));
	builder.append_query(U("recognize_vehicle"), U("0"));
	builder.append_query(U("country"), country_code);
	builder.append_query(U("return_image"), U("0"));
	builder.append_query(U("topn"), U("10"));
	http_client client(builder.to_string());

	// Build the http request (POST)
	http_request req;
	req.set_method(methods::POST);
	req.headers().add(L"Content-Type", L"application/json");	// URL VERSION
	req.headers().add(L"Accept", L"application/json");
	req.set_body(encoded);

	http_response httpResponse = client.request(req).get();

	// Deal with the response <-- this just prints it out but you may need to elaborate here by maybe using a json to contain the response
	if (httpResponse.status_code() == status_codes::OK)
	{
		wcout << "WORKED" << endl;
		wstring output = httpResponse.extract_utf16string().get();
		wcout << output << endl;
	}
	else {
		wcout << "DID NOT WORK" << endl;
		wcout << httpResponse.status_code() << endl;
	}

}

