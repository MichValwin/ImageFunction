#include <cstdio>
#include <cstdint>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <random>
#include "XoshiroCpp.hpp"

class Image4Channels8Bytes{
private:
	unsigned int width, height;
	const unsigned int channels = 4;

	uint8_t* imageData;
public:
	Image4Channels8Bytes(unsigned int width, unsigned int height): width(width), height(height){
		/*
		if(width * height * channels > 1000000000){
			throw "Can't create image over 10000M";
		}
		*/

		//Alocate and put to 0 all Bytes
		imageData = (uint8_t*)calloc(width * height * channels, sizeof(uint8_t));
		if(!imageData)throw "Can't allocate memory";

		printf("Created internal image data, width: %d, height: %d, channels: %d. Totalsize: %dKB\n", this->width, this->height, this->channels, (int)((width * height * channels) / 1024.0f));
	}

	~Image4Channels8Bytes(){
		if(imageData != nullptr)free(imageData);
	}

	int setColor(unsigned int x, unsigned int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
		if(x >= this->width || y >= this->height)return -1;

		unsigned int pixelToArray = (x * this->channels) + (y * this->width * this->channels);

		imageData[pixelToArray + (unsigned int)0] = r;
		imageData[pixelToArray + (unsigned int)1] = g;
		imageData[pixelToArray + (unsigned int)2] = b;
		imageData[pixelToArray + (unsigned int)3] = a;

		return 1;
	}

	int writeToFilePNG(const char* fileName){
		return stbi_write_png(fileName, this->width, this->height, this->channels, this->imageData, this->width * this->channels);
	}

	int writeToFileBMP(const char* fileName){
		return stbi_write_bmp(fileName, this->width, this->height, this->channels, this->imageData);
	}

	void emptyImage(){
		memset(imageData, 0, width * height * channels * sizeof(uint8_t));
	}

	uint8_t* getImageDataBuffer() const{
		return this->imageData;
	}

	unsigned int getWidth() const{
		return this->width;
	}

	unsigned int getHeight() const{
		return this->height;
	}

	unsigned int getChannels() const{
		return this->channels;
	}
};


void iterateNormalFern(uint8_t* arratData, unsigned int width, unsigned int height, unsigned long long numIterations, int srandGen){
	double x = 0;
	double y = 0;
	
	//Create random fern
	//srand(srandGen);
	//XoshiroCpp::Xoshiro256PlusPlus rng(srandGen);


	for(unsigned long long i = 0; i < numIterations; i++){
		double nextX, nextY;
		double r = (double)rand() / (double)RAND_MAX;
		//double r = XoshiroCpp::DoubleFromBits(rng());

		if (r < 0.01) {
			nextX =  0.0;
			nextY =  0.16 * y;
		} else if (r < 0.86) {
			nextX =  0.85 * x + 0.04 * y;
			nextY = -0.04 * x + 0.85 * y + 1.6;
		} else if (r < 0.93) {
			nextX =  0.20 * x - 0.26 * y;
			nextY =  0.23 * x + 0.22 * y + 1.6;
		} else {
			nextX = -0.15 * x + 0.28 * y;
			nextY =  0.26 * x + 0.24 * y + 0.44;
		}

		// Scaling and positioning
		double plotX = width * (x + 3.0) / 6.0;
		double plotY = height - height * ((y + 2.0) / 14.0);

		unsigned int positionArr = (unsigned int)plotX + (unsigned int)plotY * width;
		arratData[positionArr] = 1;

		x = nextX;
		y = nextY;
	}
}

int main(){
	unsigned int width = 1000;
	unsigned int height = 1000;
	
	const int numThreads = 4;
	const unsigned long long numIter = (unsigned long long)UINT_MAX / (unsigned long long)200;
	
	try{
		//Set image
		Image4Channels8Bytes image = Image4Channels8Bytes(width, height);

		
		//Create threads
		std::thread threads[numThreads];
		uint8_t* dataArrays[numThreads];
		for(int i = 0; i < numThreads; i++){
			dataArrays[i] = (uint8_t*)calloc(width*height, sizeof(uint8_t));
		}
		printf("Using: %dKB memory for the %d threads\n", (unsigned int)((width*height) / 1024.0), numThreads);
		printf("Each thread is doing %llu iters\n", numIter);

		for(int i = 0; i < numThreads; i++){
			threads[i] = std::thread(iterateNormalFern, dataArrays[i], width, height, numIter, i * 542334);
		}

		//iterateFern(data, width, height, 1000000, 100);
		
		for(int i = 0; i < numThreads; i++){
			threads[i].join();
		}

		printf("All threads joined...\n");

		//Vuelca datos de cada hilo
		
		for(int k = 0; k < numThreads; k++){
			for(unsigned int i = 0; i < height; i++){
				for(unsigned int j = 0; j < width; j++){

					if(dataArrays[k][j + i * width] != 0){
						switch(dataArrays[k][j + i * width]){
							case 1:
								image.setColor(j, i, 0xFF, 0x00, 0x00, 0xFF);
							break;
							case 2:
								image.setColor(j, i, 0x00, 0xFF, 0x00, 0xFF);
							break;
							case 3:
								image.setColor(j, i, 0x00, 0x00, 0xFF, 0xFF);
							break;
							case 4:
								image.setColor(j, i, 0xAA, 0xAA, 0x22, 0xFF);
							break;
						}
					}
				}	
			}
			char str[80];
			memset(str, 0, 80 * sizeof(char));

			sprintf(str, "filename%d.png", k);
			int ret = image.writeToFilePNG(str);
			printf("Ret from writeImage: %d while writing: %s\n", ret, str);
			image.emptyImage();
		}


		//Vuelca datos unidos
		unsigned long long countPixels = 0;

		for(int k = 0; k < numThreads; k++){
			for(unsigned int i = 0; i < height; i++){
				for(unsigned int j = 0; j < width; j++){
					if(dataArrays[k][j + i * width] != 0){
						image.setColor(j, i, 0x00, 0xFF, 0x00, 0xFF);
						countPixels++;
					}
				}	
			}
		}

		printf("Pixels drawn: %llu\n", countPixels);

		for(int i = 0; i < numThreads; i++){
			free(dataArrays[i]);
		}


		//Write file
		//int ret = image.writeToFilePNG("unidos.png");
		int ret = image.writeToFilePNG("unidos.bmp");
		printf("Ret from writeImage final: %d\n", ret);
		image.~Image4Channels8Bytes();


	}catch(const char* ex){
		printf("Error: %s\n", ex);
		return -1;
	}
	
	printf("Finished writing");
	return 0;
}