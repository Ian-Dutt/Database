#!/usr/bin/python3
import ctypes

database_dll = ctypes.CDLL("./libdatabase.so")
c_null = ctypes.c_void_p(None)

CResultTypes = {
     0 : (ctypes.c_int, ctypes.sizeof(ctypes.c_int), int),
     1 : (ctypes.c_long, ctypes.sizeof(ctypes.c_long), int),
     2 : (ctypes.c_double, ctypes.sizeof(ctypes.c_double), float),
     3 : ((ctypes.c_char * 32), ctypes.sizeof(ctypes.c_char) * 32, lambda s: s.decode('utf-8')),
     4 : (ctypes.c_char, ctypes.sizeof(ctypes.c_char), lambda s: s.decode('utf-8')),
}

class CResult(ctypes.Structure):
    _fields_ = [
        ('len', ctypes.c_size_t),
        ('cols', ctypes.c_size_t),
        ('data', ctypes.POINTER(ctypes.c_void_p)),
        ('types', ctypes.POINTER(ctypes.c_int)),
        ('column_names', ctypes.POINTER(ctypes.c_char_p)),
    ]

class Result:
    def __init__(self, result: CResult):
        self.num_cols = result.cols
        self.num_rows = result.len
    
        self.column_names = [str(name) for name in result.column_names[:self.num_cols]]
        self.types = [CResultTypes[t] for t in result.types[:self.num_cols]]

        print(self.column_names)
        # print(self.types)

        self.rows = [
            self._parse_row(res_row) for res_row in result.data[:self.num_rows]
        ]

        print(self.rows)


    def _parse_row(self, data_row):
        data_row = ctypes.cast(data_row, ctypes.c_void_p)
        row = []
        for ctype, inc, cast in self.types:
            tmp = ctypes.cast(data_row, ctypes.POINTER(ctype))

            val = tmp[0]

            if not isinstance(val, int) and not isinstance(val, float) and not isinstance(val, bytes):
                val = val.value

            row.append(cast(val))

            data_row = ctypes.cast(data_row, ctypes.c_void_p)

            data_row.value += inc

        return row

class Database:
    def __init__(self):

        self._db = c_null
        self.is_open = False
        print(self._db)
        self.run_command = database_dll.db_command
        self.run_command.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        self.run_command.restype = ctypes.c_void_p
        self.delete_db = database_dll.delete_database
        self.delete_db.argtypes = [ctypes.c_void_p]
        self.delete_db.restype = ctypes.c_void_p
        self.connect_command = "READ {}"
        database_dll.set_log_file()

    def as_c_str(self, *args) -> bytes:
        res = None
        if len(args) > 1:
            fmt, *rest = args
            res = fmt.format(*rest)
            
        else:
            res = args[0]

        return bytes(res, 'utf-8')
    
    def command(self, *args) -> ctypes.c_void_p:
        com = self.as_c_str(*args)
        print("DQL: {}".format(com.decode('utf-8')))
        return self.run_command(self._db, com)

    def connect(self, database: str) -> bool:
        if self.is_open:
            print("Connection must be manually closed before opening another one")
            return False
        
        self._db = self.command(self.connect_command, database)

        print(self._db)

        if self._db != c_null:
            print(f'Connected to database {database}')
            self.is_open = True
            return True

        return False

    def get(self, table_name, columns) -> CResult:
        cmd = 'FROM {} GET' + (' {}' * len(columns)) + ' ;'
        
        c_result = ctypes.cast(
            self.command(cmd, table_name, *columns), 
            ctypes.POINTER(CResult)
        )

        py_result = Result(c_result.contents)

        database_dll.delete_result(c_result)

        return py_result

    def show(self, tables: str) -> bool:
        if not self.is_open:
            print('Database has not been connected too')
            return False
        
        self.command("SHOW {}", tables)
        return True

    def save(self) -> bool:
        if not self.is_open:
            print('Database has not been connected too')
            return False

        self.command("SAVE")
        return True

    def close(self) -> bool:
        if not self.is_open:
            print('Database has not been connected too')
            return False

        if self._db == c_null:
            return False

        self.delete_db(self._db)
        self._db = c_null
        self.is_open = False
        print('Closed database connection')
        return True
