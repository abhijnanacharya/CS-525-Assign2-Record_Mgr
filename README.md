# CS-525 Record Manager

## Contributors:

    - Abhijnan Acharya A20536263
    - Tanushree Halder A20554883
    - Roshan Hyalij A20547441
    - Rana Feyza Soylu A20465152

---

---

# record_mgr.c


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

This file describes the freeSchema function for releasing memory allocated for a schema.


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

This file describes the deleteRecord function for deleting an existing record from the table.


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

