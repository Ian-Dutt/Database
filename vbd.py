# view da base
import tkinter

class VDB:
    def __init__(self, width, height):
        self.width = width
        self.height = height

        self.window = tkinter.Tk()
        self.window.geometry(f'{width}x{height}')

        self.window.title('View Da Database')
    
    def start(self):
        self.window.mainloop()

def main():
    gui = VDB(800, 600)

    gui.start()

if __name__ == '__main__':
    main()