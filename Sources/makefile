#///////////////////////////////////////////////////////////////////////////////
#                          P7.Trace library                                   //
#///////////////////////////////////////////////////////////////////////////////
#To compile successfully you should respect next folder location:
#  -P7
#    -Headers
#    -Sources
#    -Tests
#  -Shared 

#mkdir -p ./../Binaries
#mkdir -p ./../Binaries/Temp

# Environment
TEMP_DIR=Temp
BIN_DIR=./../Binaries
H1_DIR=./../Shared
H2_DIR=$(H1_DIR)/Platforms/Linux_x86
H3_DIR=./../Headers
OBJ_DIR=${BIN_DIR}/${TEMP_DIR}

$(BIN_DIR)/libP7.a:$(OBJ_DIR)/Trace.o $(OBJ_DIR)/CRC32.o $(OBJ_DIR)/Client.o $(OBJ_DIR)/Telemetry.o
	ar -rv $(BIN_DIR)/libP7.a $(OBJ_DIR)/Trace.o $(OBJ_DIR)/CRC32.o $(OBJ_DIR)/Client.o $(OBJ_DIR)/Telemetry.o

$(OBJ_DIR)/Trace.o:Trace.cpp $(OBJ_DIR)/CRC32.o $(OBJ_DIR)/Client.o
	g++ -Wall -c -O2 -I$(H1_DIR) -I$(H2_DIR) -I$(H3_DIR) -MMD -MP -MF $(OBJ_DIR)/Trace.o.d -o $(OBJ_DIR)/Trace.o Trace.cpp

$(OBJ_DIR)/Telemetry.o:Telemetry.cpp $(OBJ_DIR)/CRC32.o $(OBJ_DIR)/Client.o
	g++ -Wall -c -O2 -I$(H1_DIR) -I$(H2_DIR) -I$(H3_DIR) -MMD -MP -MF $(OBJ_DIR)/Telemetry.o.d -o $(OBJ_DIR)/Telemetry.o Telemetry.cpp

$(OBJ_DIR)/Client.o:Client.cpp $(OBJ_DIR)/CRC32.o
	g++ -Wall -c -O2 -I$(H1_DIR) -I$(H2_DIR) -I$(H3_DIR) -MMD -MP -MF $(OBJ_DIR)/Client.o.d -o $(OBJ_DIR)/Client.o Client.cpp

$(OBJ_DIR)/CRC32.o:CRC32.cpp
	rm -f -r $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)
	g++ -Wall -c -O2 -I$(H1_DIR) -I$(H2_DIR) -MMD -MP -MF $(OBJ_DIR)/CRC32.o.d -o $(OBJ_DIR)/CRC32.o CRC32.cpp

clean:
	rm -f -r $(OBJ_DIR)
	
