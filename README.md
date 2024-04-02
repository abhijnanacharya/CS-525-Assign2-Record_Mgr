# CS-525 Record Manager

## Contributors:

    - Abhijnan Acharya A20536263
    - Tanushree Halder A20554883
    - Roshan Hyalij A20547441
    - Rana Feyza Soylu A20465152

---

---

The record manager handles tables with a fixed schema. Clients can insert records, delete records, update records, and scan through the records in a table.

---

## Description

**Function initRecordManager**

This file describes the `initRecordManager` function to initialize components of the record manager.

**Function Signature**

```c
extern RC initRecordManager(void *mgmtData)
```

**Purpose:** This function initializes the record manager.

**Parameters:**

- `mgmtData`: A pointer to management data can be utilized for passing configuration options or necessary state required for initialization.

**Return:**

- `RC_OK`: Indicates successful execution of the function.

**Details:**

1. Initializes global data structures or resources required by the record manager to operate. This could involve tasks such as allocating memory for management structures, setting up essential background services, or loading configuration settings.
2. If `mgmtData` is utilized, the function will interpret the provided data to configure the record manager in accordance with the requirements of the application.
3. Prepares the record manager to create, open, and manage tables and records.
4. May also entail initializing dependencies, like the storage manager or buffer manager, to ensure they are prepared for use by the record manager.
5. Returns `RC_OK` upon successful initialization, signifying that the record manager is prepared to process requests.

---

**Function shutdownRecordManager**

This file describes the `shutdownRecordManager` function to shut down or terminate the record manager

**Function Signature:**

```c
extern RC shutdownRecordManager()
```

**Purpose:** This function shuts down the record manager and releases all resources it has allocated.

**Parameters:** None.

**Return:**

- `RC_OK`: Indicates successful execution of the function.

**Details:**

1. Releases any global data structures or resources that were allocated or opened during the initialization or regular operation of the record manager.
2. Guarantees that all managed tables are closed correctly and any unsaved data is saved to disk to prevent data loss.
3. If the record manager utilizes external services or systems (such as a storage manager or a buffer manager), this function ensures that those are also properly terminated or released.
4. Executes cleanup tasks like freeing memory allocated for management structures and ensuring there are no memory leaks.
5. Following the execution of this function, the record manager will be unable to manage tables or records until `initRecordManager` is invoked again to reinitialize it.
6. Returns `RC_OK` to signify the successful completion of the shutdown process and appropriate release of all resources.

---

**Function createTable**

This file describes the `createTable` function to create a new table

**Function Signature:**

```c
extern RC createTable(char *name, Schema *schema);
```

**Purpose:** This function creates a new table with a specified name and schema.

**Parameters:**

- `name`: A string representing the name of the table to be created.
- `schema`: A pointer to a `Schema` structure that defines the schema of the table, including the types, names, and sizes of the columns, along with any key attributes.

**Return:**

- `RC_OK`: Indicates successful creation of the table.

**Details:**

1. Initializes the required data structures for representing a new table within the record manager's system. This involves allocating memory for the table's management data and configuring it according to the provided schema.
2. Generates a new page file for the table, utilizing the table's name as the identifier. This file serves as the storage location for the records associated with the table.
3. Records initial metadata to the page file, comprising schema information and any other essential management details like the number of records or structures for managing free space.
4. Serializes the schema and stores it in a dedicated section of the table's page file or management structures, enabling retrieval and interpretation during table operations.
5. Updates the record manager's internal catalog or directory with information about the newly created table, facilitating operations such as insertions, deletions, and scans on the table.
6. If the table creation process is successful and the table is ready for use, the function returns `RC_OK`. Any encountered errors during the process result in an appropriate error code being returned to indicate the failure and its cause.

---

**Function openTable**

This file describes `openTable` function to initiate access to a specified table

**Function Signature:**

```c
extern RC openTable(RM_TableData *rel, char *name);
```

**Purpose:** This function opens an existing table for operations like insertions, deletions, updates, and scans.

**Parameters:**

- `rel`: A pointer to an `RM_TableData` structure where information about the opened table will be stored.
- `name`: A string representing the name of the table to be opened.

**Return:**

- `RC_OK`: Indicates the table was successfully opened.

**Details:**

1. Retrieves the metadata of the table from the storage system, utilizing the provided table name as the identifier. This process involves accessing and reading the schema along with any additional management data essential for table operations.
2. Initializes the `RM_TableData` structure using the retrieved metadata. This includes configuring the schema pointer to accurately represent the table's schema and filling in the necessary management data fields.
3. If the table has a dedicated page file, this function ensures the file is accessible and ready for data operations. This may involve interfacing with the underlying storage manager to open the file.
4. Validates the presence of the table and its accessibility. If the table is not found or cannot be opened due to issues such as permissions or corruption, an error code is returned.
5. Establishes any internal structures or state within the record manager that are necessary for monitoring ongoing operations on the table, such as maintaining open scans or tracking buffered changes.
6. Signals the successful opening of the table and its readiness for data operations by returning `RC_OK`. In case of failure during any step of the process, an appropriate error code is returned to signify the issue.

---

**Function closeTable**

This file describes `closeTable` function to terminate access to a specific table, release associated resources and finalize operations related to it.

**Function Signature:**

```c
extern RC closeTable(RM_TableData *rel);
```

**Purpose:** This function closes an opened table, ensuring that all changes are flushed to storage and all resources are released.

**Parameters:**

- `rel`: A pointer to an `RM_TableData` structure that represents the table to be closed.

**Return:**

- `RC_OK`: Indicates successful closure of the table.

**Details:**

1. Records any modifications or updates made to the table back to the storage system. This includes flushing buffered records, updating metadata, and ensuring the schema and management information remain current.
2. Releases any resources allocated for managing the table within the record manager. This may involve freeing memory utilized for the schema, management data, or any buffers and temporary structures.
3. Shuts down the page file linked with the table, if it was opened, ensuring all data is securely stored. This typically involves invoking functionality in the storage manager to properly close the file.
4. Adjusts the record manager's internal state to indicate that the table is no longer open. This might entail removing the table from an internal directory or list of open tables.
5. Following the execution of this function, the table becomes unavailable for data operations until it is reopened using `openTable`.
6. Returns `RC_OK` to signify the successful closure of the table and the clean release of all resources. If any issues arise during the closure process, an appropriate error code is returned to indicate the problem.

---

**Function deleteTable**

This file describes `deleteTable` function to remove a table from the database, including its associated data and metadata.

**Function Signature:**

```c
extern RC deleteTable(char *name);
```

**Purpose:** This function deletes a table along with its associated data from the system.

**Parameters:**

- `name`: A string representing the name of the table to be deleted.

**Return:**

- `RC_OK`: Indicates the table was successfully deleted.

**Details:**

1. Locates the table by the given name, which entails searching through the system's catalog or directory of tables to locate the relevant table data and page file.
2. Eliminates the table's metadata and schema information from the record manager's internal structures, effectively deregistering the table from the system.
3. Deletes the associated page file linked with the table, ensuring the removal of all records and data stored within the table. This action may involve invoking functions from the storage manager or file system to delete the file.
4. Clears any in-memory data structures, caches, or buffers pertaining to the table within the record manager, thereby releasing resources and preventing access to outdated data.
5. Upon completion of this function, the table and all its records are permanently erased, making the table's name available for reuse in creating new tables.
6. Returns `RC_OK` to signify the successful deletion. If the table cannot be located or if there are issues accessing the file system to delete the table's data, an appropriate error code is returned to indicate the failure.

---

**Function getNumTuples**

This file describes `getNumTuples` function to return the total number of tuples present in a table

**Function Signature:**

```c
extern int getNumTuples(RM_TableData *rel);
```

**Purpose:** This function retrieves the number of tuples (records) currently stored in the specified table.

**Parameters:**

- `rel`: A pointer to an `RM_TableData` structure representing the table for which the number of tuples is to be retrieved.

**Return:**

- An integer representing the number of tuples (records) in the table.

**Details:**

1. Retrieves the metadata of the table, which includes details regarding the number of tuples stored in the table.
2. Provides the current count of tuples stored in the table.
3. The count can be directly obtained from the table's metadata, which is typically maintained by the record manager to monitor the number of tuples in the table.
4. This function exclusively retrieves and returns the count of tuples without modifying any data within the table or the record manager.
5. In case the table is empty, the function returns 0.

---

**Function insertRecord**

This file describes `insertRecord` function to insert a new record into a table

**Function Signature:**

```c
extern RC insertRecord(RM_TableData *rel, Record *record)
```

**Purpose:** This function inserts a new record into the table.

**Parameters:**

- `rel`: The table data structure.
- `record`: A pointer to the record to be inserted.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
  
**Details:**

1.Find an empty slot in the table for the new record by examining the table's metadata.
2.Use the getRecordSize function to calculate the size of the new record and ensure it fits on a page.
3.Locate a page with enough free space for the record, or allocate a new page if necessary.
4.Create a Record ID (RID) for the new record by combining the page number and slot number where it's inserted.
5.Copy the data from the new record into the designated slot on the page.
6.Mark the page as dirty to indicate that changes need to be written to disk.
7.Release the lock on the page by unpinning it.
8.If the insertion is successful, return RC_OK.

---

**Function deleteRecord**

This file describes `deleteRecord` function to remove records from a table

**Function Signature:**

```c
extern RC deleteRecord(RM_TableData *rel, RID id)
```

**Purpose:** This function deletes a record from the table.

**Parameters:**

- `rel`: The table data structure.
- `id`: The Record Identifier (RID) of the record to be deleted.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
  
**Details:**

1.Retrieves the table's management data to locate the page and slot of the record to be deleted.
2.Pins the page containing the target record.
3.Marks the record as deleted, typically by setting a flag in the record's metadata or by updating a table-level data structure to indicate that the slot is now free.
4.Marks the page as dirty to indicate changes.
5.Unpins the page.
6.Updates any necessary metadata to reflect the deletion, such as adjusting the count of records or updating space management data structures.
7.Returns RC_OK if the delete operation is successful.

---


**function getRecordSize**

This file describes the `getRecordSize` function for retrieving the record size.

**Function Signature:**

```c
extern int getRecordSize(Schema *schema)
```

**Purpose:** This function calculates the size of a record in bytes based on the data types and lengths specified in a schema.

**Parameters:**

- `schema`: The schema being used

**Return:**

- `recordSizeCount + 1`: This function returns the size of a record in bytes, including one additional byte.

**Details:**

1. Declares variables to store sizes and counters.
2. Iterates over each attribute in the schema.
3. Checks the data type of each attribute.
4. Adds the size of the data type to the total record size for integer, float, and boolean attributes.
5. Adds the type length to the total record size for string attributes.
6. Increments the loop counter for the next attribute.
7. Returns the total record size, including one additional byte.

---


**function freeSchema**

This file describes the `freeSchema` function for releasing memory allocated for a schema.


**Function Signature:**

```c
extern RC freeSchema(Schema *schema)
```

**Purpose:** This function releases the memory allocated for a schema.

**Parameters:**

- `schema`: The schema to be freed

**Return:**

- `RC_OK`: Indicates successful execution of the function.

**Details:**
1. Checks if the schema pointer is already NULL, indicating that it's already freed, in which case it returns RC_OK.
2. Releases the memory occupied by the schema pointer.
3. Sets the schema pointer to NULL.
4. Returns RC_OK to indicate successful execution.

---


**function deleteRecord**

This file describes the `deleteRecord` function for deleting an existing record from the table.


**Function Signature:**

```c
extern RC deleteRecord(RM_TableData *rel, RID id)
```

**Purpose:** This function deletes an existing record from the table.

**Parameters:**
- `rel`: The table data structure.
- `id`: The record identifier (RID) of the record to be deleted.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_FATAL_ERROR`: Indicates a critical error occurred during the deletion process.


**Details:**
1. Declares a variable to store the record size.
2. Retrieves the record controller details from the management data of the table.
3. Pins the page containing the record to be updated.
4. Calculates the size of the corresponding record using the getRecordSize function.
5. Updates the recordSize variable with the size obtained.
6. Marks the page containing the record as a free page.
7. Determines the location of the record to be deleted within the page.
8. Marks the record as deleted using a tombstone symbol ('-').
9. Marks the page as dirty to reflect changes.
10. Unpins the page.
11. Returns RC_OK if the deletion is successful, otherwise returns RC_FATAL_ERROR.

---

**function getRecord**

This file describes the `getRecord` function for retrieving an existing record from the table.

**Function Signature:**

```c
extern RC getRecord(RM_TableData *rel, RID id, Record *record)
```

**Purpose:** This function retrieves an existing record from the table.

**Parameters:**

- `rel`: The table data structure.
- `id`: The record identifier (RID) of the record to be retrieved.
- `record`: A pointer to the structure where the retrieved record will be stored.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_RM_NO_TUPLE_WITH_GIVEN_RID`: Indicates no record matches the given RID.

**Details:**

1. Retrieves the record controller details from the management data of the table.
2. Pins the page containing the record to be updated.
3. Determines the location of the record to be retrieved within the page.
4. Calculates the size of the corresponding record using the `getRecordSize` function.
5. Updates the `recordSize` variable with the size obtained.
6. Checks if the record exists at the specified RID; if not, returns `RC_RM_NO_TUPLE_WITH_GIVEN_RID`.
7. Copies the data of the retrieved record.
8. Unpins the page.
9. Returns `RC_OK` if the retrieval is successful.

---

**function startScan**

This file describes the `startScan` function for initializing all attributes of `RM_ScanHandle`.

**Function Signature:**

```c
extern RC startScan(RM_TableData *rel, RM_ScanHandle *scan, Expr *cond)
```

**Purpose:** This function initializes all attributes of `RM_ScanHandle` for scanning a table.

**Parameters:**

- `rel`: The table data structure.
- `scan`: A pointer to `RM_ScanHandle` structure to be initialized.
- `cond`: The condition expression for scanning.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_SCAN_CONDITION_NOT_FOUND`: Indicates a condition for scanning is not found.
- `RC_TABLE_ERROR`: Indicates an error occurred while opening the table for scanning.

**Details:**

1. Checks if the condition expression is not NULL; if NULL, returns `RC_SCAN_CONDITION_NOT_FOUND`.
2. Opens the table for scanning; if successful, proceeds with initialization.
3. Retrieves the record controller details from the management data of the table.
4. Sets the total record count to `ATTRIBUTE_SIZE`.
5. Allocates memory for a new `RM_RecordController` structure for the scan.
6. Assigns the newly allocated memory to `scan->mgmtData`.
7. Sets the `rel` pointer of the scan handle to the table data structure.
8. Sets the condition expression for scanning if provided.
9. Initializes the slot number to 0 (starting from slot 1).
10. Initializes the total scan count to 0.
11. Initializes the page number to 1 (starting from page 1).
12. Returns `RC_OK` if the initialization is successful, otherwise returns `RC_TABLE_ERROR`.

---

**function next**

This file describes the `next` function for advancing the scan to the next record.

**Function Signature:**

```c
extern RC next(RM_ScanHandle *scan, Record *record)
```

**Purpose:** This function advances the scan to the next record.

**Parameters:**

- `scan`: A pointer to `RM_ScanHandle` structure representing the scan.
- `record`: A pointer to the structure where the retrieved record will be stored.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_SCAN_CONDITION_NOT_FOUND`: Indicates a condition for scanning is not found.
- `RC_RECORD_SIZE_FATAL_ERR`: Indicates a fatal error occurred while obtaining the record size.
- `RC_RM_NO_MORE_TUPLES`: Indicates there are no more tuples to scan.
- `RC_FATAL_ERROR`: Indicates a critical error occurred during the scan.

**Details:**

1. Retrieves the record controller details from the scan's management data.
2. Checks if the condition expression for scanning is not NULL; if NULL, returns `RC_SCAN_CONDITION_NOT_FOUND`.
3. Retrieves the record controller details from the table's management data.
4. Retrieves the schema from the table data structure.
5. Calculates the size of the corresponding record using the `getRecordSize` function.
6. Checks if the obtained record size is 0; if 0, returns `RC_RECORD_SIZE_FATAL_ERR`.
7. Calculates the total number of slots per page.
8. Retrieves the total number of tuples in the table.
9. Initializes the scan count from the scan controller.
10. Loops through each tuple to scan.
11. Advances the slot and page numbers for scanning the next record.
12. Pins the page to update.
13. Retrieves the data of the page and calculates the offset for the current record.
14. Updates the record identifier with the page and slot numbers.
15. Marks the record as deleted using a tombstone symbol ('-') and copies the data.
16. Increments the scan count and checks the condition expression.
17. Unpins the page and returns `RC_OK` if the condition is satisfied.
18. Returns `RC_FATAL_ERROR` if any critical error occurs during the scan.
19. Resets the values if no more tuples are found and returns `RC_RM_NO_MORE_TUPLES`.

---

**function closeScan**

This file describes the `closeScan` function for cleaning up resources after scanning.

**Function Signature:**

```c
extern RC closeScan(RM_ScanHandle *scan)
```

**Purpose:** This function indicates that all resources used for scanning can now be cleaned up.

**Parameters:**

- `scan`: A pointer to `RM_ScanHandle` structure representing the scan.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_FATAL_ERROR`: Indicates a critical error occurred during the cleanup process.

**Details:**

1. Retrieves the record controller details from the table's management data.
2. Retrieves the record controller details from the scan's management data.
3. Loops through each scanned record.
4. Unpins the page containing the record.
5. Resets the values for closing.
6. Frees the memory allocated for the scan handle.
7. Returns `RC_OK` if the cleanup is successful, otherwise returns `RC_FATAL_ERROR`.

---

**function createSchema**

This file describes the `createSchema` function for creating a new schema and initializing all attributes.

**Function Signature:**

```c
extern Schema *createSchema(int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys)
```

**Purpose:** This function creates a new schema and initializes all attributes.

**Parameters:**

- `numAttr`: The number of attributes in the schema.
- `attrNames`: An array of attribute names.
- `dataTypes`: An array of data types for each attribute.
- `typeLength`: An array specifying the length of each attribute (for string types).
- `keySize`: The size of the key.
- `keys`: An array of key attributes.

**Return:**

- `currschema`: A pointer to the newly created schema if successful.
- `RC_SCHEMA_CREATION_FAILURE`: Indicates failure to create the schema.

**Details:**

1. Checks if the size of `Schema` structure is greater than or equal to 0.
2. Allocates memory for a new `Schema` structure.
3. Checks if the allocation was successful.
4. Sets up the attributes of the schema:
   - `numAttr`: Number of attributes.
   - `attrNames`: Array of attribute names.
   - `dataTypes`: Array of data types for each attribute.
   - `typeLength`: Array specifying the length of each attribute (for string types).
   - `keySize`: Size of the key.
   - `keys`: Array of key attributes.
5. Returns the pointer to the newly created schema if successful, otherwise returns `RC_SCHEMA_CREATION_FAILURE`.

---

**function updateRecord**

This file describes the `updateRecord` function for updating a record.

**Function Signature:**

```c
extern RC updateRecord(RM_TableData *rel, Record *rec)
```

**Purpose:** This function updates a record.

**Parameters:**

- `rel`: The table data structure.
- `rec`: A pointer to the record to be updated.

**Return:**

- `RC_OK`: Indicates successful execution of the function.

**Details:**

1. Retrieves the record controller details from the table's management data.
2. Pins the page containing the record to be updated.
3. Calculates the size of the corresponding record using the `getRecordSize` function.
4. Retrieves the record identifier (RID) from the record.
5. Calculates the offset for the current record.
6. Retrieves the data of the page and updates the record status to '+' (indicating it's not deleted).
7. Copies the data from the record to the page.
8. Marks the page as dirty to reflect changes.
9. Unpins the page.
10. Returns `RC_OK` if the update is successful.

---

**function createRecord**

This file describes the `createRecord` function for creating a new record in the schema.

**Function Signature:**

```c
extern RC createRecord(Record **record, Schema *schema)
```

**Purpose:** This function creates a new record in the schema.

**Parameters:**

- `record`: A pointer to a pointer to `Record` structure where the newly created record will be stored.
- `schema`: The schema structure.

**Return:**

- `RC_OK`: Indicates successful execution of the function.

**Details:**

1. Retrieves the size of the corresponding record using the `getRecordSize` function.
2. Allocates memory for a new `Record` structure.
3. Allocates memory for the data of the record.
4. Initializes the page and slot identifiers of the record to -1.
5. Initializes the first byte of the record's data to indicate it's not deleted.
6. Sets the data pointer to the allocated memory.
7. Updates the pointer to the newly created record.
8. Returns `RC_OK` to indicate successful creation of the record.

---

**function freeRecord**

This file describes the `freeRecord` function for freeing the memory allocated for a record.

**Function Signature:**

```c
extern RC freeRecord(Record *record)
```

**Purpose:** This function frees the memory allocated for a record.

**Parameters:**

- `record`: A pointer to the record structure to be freed.

**Return:**

- `RC_OK`: Indicates successful execution of the function.

**Details:**

1. Checks if the record pointer is not NULL.
2. Sets the record pointer to NULL to prevent dangling pointers.
3. Frees the memory allocated for the record.
4. Returns `RC_OK` to indicate successful freeing of the record memory.

---

**function attrOffset**

This file describes the `attrOffset` function for calculating the offset associated with an attribute in a schema.

**Function Signature:**

```c
RC attrOffset(Schema *schema, int attrNum, int *res)
```

**Purpose:** This function calculates the offset associated with an attribute in a schema.

**Parameters:**

- `schema`: The schema structure.
- `attrNum`: The attribute number for which the offset is calculated.
- `res`: A pointer to an integer where the result of the calculation will be stored.

**Return:**

- `RC_OK`: Indicates successful execution of the function.

**Details:**

1. Declares variables to store sizes and iterators.
2. Initializes the result to 1.
3. Iterates through each attribute until reaching the specified attribute number.
4. Determines the data type of the attribute and adds the corresponding size to the result.
5. Prints "Valid!" if the result is greater than or equal to -1 (debugging purposes).
6. Returns `RC_OK` to indicate successful calculation of the attribute offset.

---


**function getAttr**

This file describes the `getAttr` function for retrieving the attribute values of a record.

**Function Signature:**

```c
extern RC getAttr(Record *rec, Schema *schema, int attrNum, Value **value)
```

**Purpose:** This function retrieves the attribute values of a record.

**Parameters:**

- `rec`: A pointer to the record structure.
- `schema`: The schema structure.
- `attrNum`: The attribute number for which the value is retrieved.
- `value`: A pointer to a pointer to `Value` structure where the retrieved value will be stored.

**Return:**

- `RC_OK`: Indicates successful execution of the function.

**Details:**

1. Initializes the offset to 0.
2. Calculates the offset associated with the specified attribute using the `attrOffset` function.
3. Prints "OFFSET CALCULATED SUCCESSFULLY!" if the offset calculation is successful.
4. Allocates memory for a new `Value` structure.
5. Retrieves the data pointer of the record.
6. Moves the data pointer to the calculated offset.
7. Checks the data type of the attribute:
   - If it is `DT_INT`, retrieves the integer value.
   - If it is `DT_STRING`, retrieves the string value.
   - If it is `DT_FLOAT`, retrieves the float value.
   - If it is `DT_BOOL`, retrieves the boolean value.
8. Copies the retrieved value to the `Value` structure.
9. Updates the pointer to the retrieved value.
10. Returns `RC_OK` to indicate successful retrieval of the attribute value.

---

**function setAttr**

This file describes the `setAttr` function for setting the attribute values of a record.

**Function Signature:**

```c
extern RC setAttr(Record *rec, Schema *schema, int attrNum, Value *value)
```

**Purpose:** This function sets the attribute values of a record.

**Parameters:**

- `rec`: A pointer to the record structure.
- `schema`: The schema structure.
- `attrNum`: The attribute number for which the value is set.
- `value`: A pointer to the `Value` structure containing the value to be set.

**Return:**

- `RC_OK`: Indicates successful execution of the function.
- `RC_SERIALIZER_ERROR`: Indicates an error occurred during serialization.

**Details:**

1. Initializes the offset to 0.
2. Calculates the offset associated with the specified attribute using the `attrOffset` function.
3. Retrieves the data pointer of the record.
4. Moves the data pointer to the calculated offset.
5. Checks the data type of the attribute:
   - If it is `DT_INT`, sets the integer value.
   - If it is `DT_STRING`, sets the string value.
   - If it is `DT_FLOAT`, sets the float value.
   - If it is `DT_BOOL`, sets the boolean value.
6. Returns `RC_OK` if the value is successfully set, otherwise returns `RC_SERIALIZER_ERROR`.

