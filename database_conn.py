import ctypes

database_dll = ctypes.CDLL(".\libdatabase.dll")

print(database_dll)