#include <iostream>
#include <pqxx/pqxx>

class postgresql_db_lite_editor 
{
public:
    postgresql_db_lite_editor() {};
    postgresql_db_lite_editor(std::string&& str)
    {
        conPtr = std::make_unique<pqxx::connection>(str);
    };

    void createTable()
    {
        std::unique_ptr<pqxx::work> tnxPtr = std::make_unique<pqxx::work>(*conPtr);
        tnxPtr->exec
        (
            "CREATE TABLE IF NOT EXISTS Clients("
            "id SERIAL PRIMARY KEY, "
            "name VARCHAR(30) NOT NULL, "
            "lastname VARCHAR(30) NOT NULL)"
        );

        tnxPtr->exec
        (
            "CREATE TABLE IF NOT EXISTS Emails("
            "id SERIAL PRIMARY KEY, "
            "client_id INTEGER NOT NULL REFERENCES Clients(id), "
            "email VARCHAR(80))"
        );

        tnxPtr->exec
        (
            "CREATE TABLE IF NOT EXISTS Cell_numbers("
            "id SERIAL PRIMARY KEY, "
            "client_id INTEGER NOT NULL REFERENCES Clients(id), "
            "cell_number VARCHAR(20))"
        );

        tnxPtr->commit();
    }

    void addClient(std::string&& name, std::string&& lastname) 
    {
        std::unique_ptr<pqxx::work> tnxPtr = std::make_unique<pqxx::work>(*conPtr);
        
        tnxPtr->exec
        (
           "INSERT INTO Clients(name, lastname) "
                "VALUES('" + tnxPtr->esc(name) + "', '" + tnxPtr->esc(lastname) + "')"
        );

        tnxPtr->commit();
    }

    void addEmail(std::string&& client_id, std::string&& email) 
    {
        std::unique_ptr<pqxx::work> tnxPtr = std::make_unique<pqxx::work>(*conPtr);

        tnxPtr->exec
        (
            "INSERT INTO Emails(client_id, email) "
            "VALUES(" + tnxPtr->esc(client_id) + ", '" + tnxPtr->esc(email) + "')"
        );

        tnxPtr->commit();
    }

    void addCellNumber(std::string&& client_id, std::string&& cellNum)
    {
        std::unique_ptr<pqxx::work> tnxPtr = std::make_unique<pqxx::work>(*conPtr);

        tnxPtr->exec
        (
            "INSERT INTO Cell_numbers(client_id, cell_number) "
            "VALUES(" + tnxPtr->esc(client_id) + ", '" + tnxPtr->esc(cellNum) + "')"
        );

        tnxPtr->commit();
    }

    void updateClient(std::string&& id, std::string&& newName, std::string&& newLastname) 
    {
        std::unique_ptr<pqxx::work> tnxPtr = std::make_unique<pqxx::work>(*conPtr);

        tnxPtr->exec
        (
            "UPDATE Clients "
            "SET name='" + tnxPtr->esc(newName) + "' WHERE id =" + tnxPtr->esc(id)
        );

        tnxPtr->exec
        (
            "UPDATE Clients "
            "SET lastname='" + tnxPtr->esc(newLastname) + "' WHERE id =" + tnxPtr->esc(id)
        );

        tnxPtr->commit();
    }

    void deleteCellNumber(std::string&& client_id) 
    {
        std::unique_ptr<pqxx::work> tnxPtr = std::make_unique<pqxx::work>(*conPtr);

        tnxPtr->exec
        (
            "DELETE FROM Cell_numbers "
            "WHERE client_id=" + tnxPtr->esc(client_id)
        );
        tnxPtr->commit();
    }

    void deleteEmail(std::string&& client_id)
    {
        std::unique_ptr<pqxx::work> tnxPtr = std::make_unique<pqxx::work>(*conPtr);

        tnxPtr->exec
        (
            "DELETE FROM Emails "
            "WHERE client_id=" + tnxPtr->esc(client_id)
        );
        tnxPtr->commit();
    }

    void deleteClient(std::string&& client_id) 
    {
        std::unique_ptr<pqxx::work> tnxPtr = std::make_unique<pqxx::work>(*conPtr);

        tnxPtr->exec
        (
            "DELETE FROM Clients "
            "WHERE id=" + tnxPtr->esc(client_id)
        );
        tnxPtr->commit();
    }

    int findClient(std::string&& name, std::string&& lastname, std::string&& cellNum, std::string&& email) 
    {
        std::unique_ptr<pqxx::work> tnxPtr = std::make_unique<pqxx::work>(*conPtr);
        int id = tnxPtr->query_value<int>
        (
            "SELECT id FROM Clients "
            "WHERE (name='" + tnxPtr->esc(name) + "' "
            "AND lastname='" + tnxPtr->esc(lastname) + "') "
            "AND (id IN (SELECT client_id FROM Cell_numbers WHERE cell_number = '" + tnxPtr->esc(cellNum) + "') "
            "OR id IN (SELECT client_id FROM Emails WHERE email = '" + tnxPtr->esc(email) + "'))"
        );

        return id;
    }

private:
    std::unique_ptr<pqxx::connection> conPtr = nullptr;
};

int main(int argc, char** argv)
{
    try 
    {
        postgresql_db_lite_editor db
        (
            "host=localhost "
            "port=5432 "
            "dbname=homework_5 "
            "user=postgres "
            "password=********"
        );
        
        db.createTable();
        db.addClient("Andrey", "Dogadaev");
        db.addCellNumber("1", "+123456789");
        db.addEmail("1", "andre.dogadaev@gmail.com");
        //db.updateClient("1", "A", "D");
        //db.deleteCellNumber("1");
        //db.deleteEmail("1");
        //db.deleteClient("1");
        std::cout << db.findClient("Andrey", "Dogadaev", "+123456789", "andre.dogadaev@gmail.com");
    }

    catch(pqxx::sql_error err)
    {
        std::cout << err.what() << std::endl;
    }

    catch (const std::exception gErr)
    {
        std::cout << gErr.what() << std::endl;
    }

    return 0;
}