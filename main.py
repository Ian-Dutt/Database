import connector

def main():
    db = connector.Database()

    db.connect("examples/Persons.db")

    db.show('ALL')
    db.show('Name')

    db.close()

if __name__ == '__main__':
    main()
