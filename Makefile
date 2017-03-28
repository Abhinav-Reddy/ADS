all:encoder decoder analysis
encoder:encoder.cpp
	g++ encoder.cpp -o encoder

decoder:decoder.cpp
	g++ decoder.cpp -o decoder

analysis:analysis.cpp
	g++ analysis.cpp -o analysis
