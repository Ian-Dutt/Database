#!/usr/bin/python3
import ctypes

database_dll = ctypes.CDLL("./libdatabase.so")
c_null = ctypes.c_void_p(None)

class Database:
    def __init__(self):

        self._db = c_null
        print(self._db)
        self.run_command = database_dll.db_command
        self.run_command.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        self.delete_db = database_dll.delete_database
        self.delete_db.argtypes = [ctypes.c_void_p]
        self.connect_command = "READ {}"
        database_dll.set_log_file()

    def as_c_str(self, fmt, *args):
        return bytes(fmt.format(*args), 'utf-8')

    def connect(self, database: str):
        command = self.as_c_str(self.connect_command, database)
        self._db = self.run_command(self._db, command)

        print(self._db)

        if self._db != c_null:
            print(f'Connected to database {database}')
            return True

        return False

    def show(self, tables: str):
        command = self.as_c_str("SHOW {}", tables)
        self.run_command(self._db, command)

    def save(self):
        if self._db == c_null:
            return False

        self.run_command(self._db, b"SAVE")
        return True

    def close(self):
        if self._db == c_null:
            return False

        self.delete_db(self._db)
        self._db = c_null
        print('Closed database connection')
        return True
