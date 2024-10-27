#!/usr/bin/python3
import ctypes

database_dll = ctypes.CDLL("./libdatabase.so")
c_null = ctypes.c_void_p(None)

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

    def as_c_str(self, *args):
        
        res = None
        if len(args) > 1:
            fmt, *rest = args
            res = fmt.format(*rest)
            
        else:
            res = args[0]

        return bytes(res, 'utf-8')
    
    def command(self, *args):
        com = self.as_c_str(*args)
        print("DQL: {}".format(com.decode('utf-8')))
        return self.run_command(self._db, com)

    def connect(self, database: str):
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

    def show(self, tables: str):
        if not self.is_open:
            print('Database has not been connected too')
            return False
        
        self.command("SHOW {}", tables)
        return True

    def save(self):
        if not self.is_open:
            print('Database has not been connected too')
            return False

        self.command("SAVE")
        return True

    def close(self):
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
