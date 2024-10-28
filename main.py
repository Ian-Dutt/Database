import connector

def main():
    db = connector.Database()

    db.connect("examples/Persons.db")

    db.show('ALL')
    db.get('Name', ['Age', 'First', 'Last', 'Middle'])
    
    # FROM Name GET Age First Last ;

    db.close()

if __name__ == '__main__':
    main()
