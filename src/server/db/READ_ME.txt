This is a database.
Each database contains many tables.
Each table contains many mini_table.
Each mini_table contains: 
            _ Two buckets: One shows rows in table, one shows field in table.
                With rows bucket: Each row contain many fields, and the value in that memory show index in field bucket show value of (row, field)
                With fields bucket: Is a 2-dimension array. One dim for field, One dim for value. (field, value) 
            _ Table info: Contain all info about table

Caution: 
        _ In hash function: index 0 is never used => size bucket = prime + 1