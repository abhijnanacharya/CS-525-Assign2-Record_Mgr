#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "record_mgr.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include "constants.h"

bool recordFeatureFlag = false;
// Structure for Record Controller for containing all the related information
typedef struct __attribute__((packed)) RM_RecordController
{
    BM_BufferPool buffPoolMgmt; // Buffer Pool Management Information & its attributes
    BM_PageHandle bmPageHandle; // This is to access Page Files
    Expr *condition;            // This is for scanning the records in the table
    RID recID;                  // For each record ID
    int freePagesNum;           // Store the freePagesNum details
    int totalScanCount;         // To store the count of the scanned records
    int totalRecordCount;       // This is to store the total number of records in the table
} RM_RecordController;

RM_RecordController *recController;

int checkRecordInitFlag()
{
    if (recordFeatureFlag == true)
        return RC_OK;
    return -1;
}

// Check if the record manager is initialized, if not it will initialize it.
RC initRecordManager(void *mgmtData)
{
    if (checkRecordInitFlag() != RC_OK)
    {
        recordFeatureFlag = true;
        printf("**********************************************************************************\n");
        printf("******************   RECORD MANAGER SUCCESSFULLY INITIALIZED   *******************\n");
        printf("**********************************************************************************\n");
        initStorageManager();
    }
    else
        printf("\n *******RECORD MANAGER ALREADY INITIALIZED*******\n");
    return RC_OK;
}

// Shutdown the record manager and set the initialization flag to 0 (reset)
RC shutdownRecordManager()
{
    printf("**********************************************************************************\n");
    printf("*********************        RECORD MANAGER SHUTDOWN         **********************\n");
    printf("**********************************************************************************\n");
    recordFeatureFlag = false;
    recController = 0;
    // Free the occupied memory
    free(recController);
    return RC_OK;
}

// Retrieve schema pointer which stores all kinds of stats values
Schema *getSchema(SM_PageHandle pageHandle)
{
    int szint = sizeof(int);
    int num_attrs = *(int *)pageHandle; // Get the number of attributes from the page file
    pageHandle += szint;

    // Allocate memory for the Schema struct
    Schema *schema_ptr = (Schema *)malloc(sizeof(Schema));

    // Initialize the Schema struct members
    schema_ptr->numAttr = num_attrs;
    schema_ptr->attrNames = malloc(sizeof(char *) * num_attrs);
    schema_ptr->dataTypes = malloc(sizeof(DataType) * num_attrs);
    schema_ptr->typeLength = malloc(sizeof(int) * num_attrs);

    // Allocate memory space for storing attribute name for each attribute
    int i = 0;
    while (i < num_attrs)
    {
        schema_ptr->attrNames[i] = (char *)malloc(ATTRIBUTE_SIZE);
        i++;
    }

    int index = 0;
    for (; index < num_attrs;)
    {
        strncpy(schema_ptr->attrNames[index], pageHandle, ATTRIBUTE_SIZE); // Assign attribute names from pageHandle
        pageHandle += ATTRIBUTE_SIZE;
        schema_ptr->dataTypes[index] = *pageHandle; // Assign attribute data type from pageHandle
        pageHandle += szint;
        schema_ptr->typeLength[index] = *pageHandle; // Assign attribute data type length from pageHandle
        pageHandle += szint;
        index++;
    }
    return schema_ptr;
}

// Creates a table of the given name and schema provided.
extern RC createTable(char *name, Schema *schema)
{ // Allocate memory for the record controller
    recController = (RM_RecordController *)calloc(1, sizeof(RM_RecordController));
    // Initialize the buffer pool
    initBufferPool(&recController->buffPoolMgmt, name, MAX_PAGES, RS_FIFO, NULL);

    // Prepare the data to be written to the first page
    char pageData[PAGE_SIZE];
    char *dataPtr = pageData;

    // Set the number of records in the page to 0
    int zeroValue = 0;
    memcpy(dataPtr, &zeroValue, sizeof(int));
    dataPtr += sizeof(int);

    // Set the page number to 1 (first page)
    int pageNum = 1;
    memcpy(dataPtr, &pageNum, sizeof(int));
    dataPtr += sizeof(int);

    // Store the number of attributes in the schema
    memcpy(dataPtr, &schema->numAttr, sizeof(int));
    dataPtr += sizeof(int);

    // Store the key size
    memcpy(dataPtr, &schema->keySize, sizeof(int));
    dataPtr += sizeof(int);

    // Store attribute details
    int index = 0;
    for (; index < schema->numAttr;)
    { // Copy the attribute names
        strncpy(dataPtr, schema->attrNames[index], ATTRIBUTE_SIZE);
        dataPtr += ATTRIBUTE_SIZE;

        // Store the data type
        memcpy(dataPtr, &schema->dataTypes[index], sizeof(DataType));
        dataPtr += sizeof(DataType);
        // Store the type length
        memcpy(dataPtr, &schema->typeLength[index], sizeof(int));
        dataPtr += sizeof(int);
        index = index + 1;
    }

    SM_FileHandle fileHandler;

    // Create and open a PageFile with the given name.
    if (createPageFile(name) != RC_OK || openPageFile(name, &fileHandler) != RC_OK)
        return RC_FILE_NOT_FOUND;

    int writeCode = writeBlock(0, &fileHandler, pageData);
    if (writeCode == RC_OK)
    {
        // Close the file and free the record controller
        return closePageFile(&fileHandler);
        recController = NULL; // dump the ptr
        free(recController);
    }
    return writeCode;
}

// Open a table of the given name and relation provided for any operation to be performed.
extern RC openTable(RM_TableData *rel, char *name)
{
    int sz = sizeof(int); // Compiler dependent x86 assuming
    // Pin the first page (page 0) of the table
    pinPage(&recController->buffPoolMgmt, &recController->bmPageHandle, 0);

    // Assign the initial pointer (starting location) to the page data
    SM_PageHandle pageHandle = (SM_PageHandle *)recController->bmPageHandle.data;

    // Get the total number of tuples from the page file
    recController->totalRecordCount = (int *)*pageHandle;
    pageHandle += sz;

    // Get the free page number from the page file
    recController->freePagesNum = *pageHandle;
    pageHandle += sz;
    
        // Set the management data pointer of the table data structure
        rel->mgmtData = (RM_RecordController *)recController;

        // Set the table name in the table data structure
        rel->name = name;

        // Get the schema for the table by parsing the page data
        rel->schema = getSchema(pageHandle);

        // Unpin the first page of the table
        printf("\n UNPINNING PAGE \n");
        unpinPage(&recController->buffPoolMgmt, &recController->bmPageHandle);

        // Force the changes to be written to disk
        printf("\n WRITING CHANGES TO DISK \n");
        forcePage(&recController->buffPoolMgmt, &recController->bmPageHandle);
    

    return RC_OK;
}

// Close a table associated with the given relation after all the operations to be performed are done.
extern RC closeTable(RM_TableData *rel)
{
    if (shutdownBufferPool(&((RM_RecordController *)rel->mgmtData)->buffPoolMgmt))
        return RC_OK;
    else
        return RC_ERROR; // FAILURE TO CLOSE BUFFER
}

// Deletes a table with the given name after all the operations to be performed are done.
extern RC deleteTable(char *name)
{
    return destroyPageFile(name);
}

// Retrieve the Total Number of Tuples in the Table
extern int getNumTuples(RM_TableData *rel)
{
    return ((RM_RecordController *)rel->mgmtData)->totalRecordCount;
}


// dealing with records and attribute values
// Function to create a new record
RC createRecord(Record **record, Schema *schema) {
    if (!schema) return RC_INVALID_INPUTS; 

    // Allocate memory for the Record structure
    *record = (Record *)calloc(1, sizeof(Record));
    if (*record == NULL) return RC_MEM_ALLOC_FAILED;
    
    int size = getRecordSize(schema);
    
    // Allocate memory for the data field to hold the binary representation of all attributes
    (*record)->data = (char *)malloc(size);
    if ((*record)->data == NULL) {
        free(*record); // Clean up previously allocated memory for the Record structure
        return RC_MEM_ALLOC_FAILED; // Error handling
    }
    
    // Initialize the data field to default values based on the attribute types
    memset((*record)->data, 0, size); 
    
    return RC_OK; // Indicate success
}

//Clearing up the record created earlier
RC freeRecord(Record *record) {
    if (record != NULL) {
        // First, free the data field of the Record, if it exists
        if (record->data != NULL) {
            free(record->data);
            record->data = NULL; // Set to NULL to avoid dangling pointer
        }

        // Now, free the Record structure itself
        free(record);
        record = NULL; // Set to NULL to avoid dangling pointer
    }
    return RC_OK;
}

// function to calculate the offset of an attribute in the record's data
int calAttrOffset(Schema *schema, int attrNum) {
    int offset = 0;
    for(int i = 0; i < attrNum; i++) {
        switch(schema->dataTypes[i]) {
            case DT_INT: offset += sizeof(int); break;
            case DT_STRING: offset += schema->typeLength[i]; break;
            case DT_FLOAT: offset += sizeof(float); break;
            case DT_BOOL: offset += sizeof(bool); break;
        }
    }
    return offset;
}

// Function to get an attribute value from a record
RC getAttr(Record *record, Schema *schema, int attrNum, Value **value) {
    if (!record || !schema || !value || attrNum < 0 || attrNum >= schema->numAttr) {
        return RC_INVALID_INPUTS; // Error handling
    }

    // Allocate memory for the Value structure
    *value = (Value *)malloc(sizeof(Value));
    if (*value == NULL) {
        return RC_MEM_ALLOC_FAILED; // Error handling
    }

    // Determine the offset and size of the attribute
    int offset = calAttrOffset(schema, attrNum);
    DataType attrType = schema->dataTypes[attrNum];

    // Initialize the Value structure
    (*value)->dt = attrType;

    // Extract and convert the binary data to a Value structure
    switch (attrType) {
        case DT_INT:
            memcpy(&((*value)->v.intV), record->data + offset, sizeof(int));
            break;

        case DT_STRING:
            // Assume typeLength includes the full string size
            (*value)->v.stringV = (char *)malloc(schema->typeLength[attrNum] + 1); // +1 for null-terminator
            memcpy((*value)->v.stringV, record->data + offset, schema->typeLength[attrNum]);
            (*value)->v.stringV[schema->typeLength[attrNum]] = '\0'; // Null-terminate the string
            break;

        case DT_FLOAT:
            memcpy(&((*value)->v.floatV), record->data + offset, sizeof(float));
            break;

        case DT_BOOL:
            memcpy(&((*value)->v.boolV), record->data + offset, sizeof(bool));
            break;

        default:
            free(*value);
            return RC_INVALID_DATATYPE; // Error handling
    }

    return RC_OK; // Success
}


// Function to set an attribute value in a record
RC setAttr(Record *record, Schema *schema, int attrNum, Value *value) {
    if (!record || !schema || attrNum < 0 || attrNum >= schema->numAttr) {
        return RC_INVALID_INPUTS;
    }

    int offset = calAttrOffset(schema, attrNum);

    switch(value->dt) {
        case DT_INT:
            memcpy(record->data + offset, &(value->v.intV), sizeof(int));
            break;

        case DT_STRING:
            memcpy(record->data + offset, value->v.stringV, schema->typeLength[attrNum]);
            break;

        case DT_FLOAT:
            memcpy(record->data + offset, &(value->v.floatV), sizeof(float));
            break;

        case DT_BOOL:
            memcpy(record->data + offset, &(value->v.boolV), sizeof(bool));
            break;

        default:
            return RC_INVALID_DATATYPE; 
    }
    
    return RC_OK; 
}