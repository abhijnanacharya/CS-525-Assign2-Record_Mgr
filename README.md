# CS-525 Record Manager

## Contributors:

    Contributors:
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

