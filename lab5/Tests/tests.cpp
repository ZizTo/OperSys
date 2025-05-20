#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "structs.h"
#include <cstring>

TEST_SUITE("Structs") {
    TEST_CASE("Employee initi") {
        Employee emp;
        emp.num = 42;
        strcpy_s(emp.name, sizeof(emp.name), "Ivan");
        emp.hours = 40.5;
        
        CHECK(emp.num == 42);
        CHECK(strcmp(emp.name, "Ivan") == 0);
        CHECK(emp.hours == 40.5);
    }
    
    TEST_CASE("Employee size validation") {
        CHECK(sizeof(Employee) == sizeof(int) + 10 * sizeof(char) + sizeof(double));
    }

    TEST_CASE("Message initialization") {
        Message msg;
        msg.type = READ_REQUEST;
        msg.employeeId = 10;
        msg.clientId = 5;
        
        CHECK(msg.type == READ_REQUEST);
        CHECK(msg.employeeId == 10);
        CHECK(msg.clientId == 5);
    }
    
    TEST_CASE("Message default values") {
        Message msg;
        CHECK(msg.employeeId == -1);
        CHECK(msg.clientId == -1);
    }
    
    TEST_CASE("Message with employee data") {
        Message msg;
        msg.type = SUCCESS_READ;
        msg.employeeId = 7;
        
        msg.employee.num = 7;
        strcpy_s(msg.employee.name, sizeof(msg.employee.name), "Maria");
        msg.employee.hours = 35.5;
        
        CHECK(msg.type == SUCCESS_READ);
        CHECK(msg.employeeId == 7);
        CHECK(msg.employee.num == 7);
        CHECK(strcmp(msg.employee.name, "Maria") == 0);
        CHECK(msg.employee.hours == 35.5);
    }
}

TEST_SUITE("Enums") {
    TEST_CASE("MessageType values") {
        CHECK(READ_REQUEST == 1);
        CHECK(WRITE_REQUEST == 2);
        CHECK(WRITE_REQUEST_READY == 3);
        CHECK(BLOCK_RESPONSE == 4);
        CHECK(SUCCESS == 5);
        CHECK(SUCCESS_READ == 6);
        CHECK(FAIL_READ == 7);
    }
}

TEST_SUITE("Server Logic Mocks") {
    std::vector<Employee> createTestEmployees() {
        std::vector<Employee> employees;
        
        Employee emp1;
        emp1.num = 0;
        strcpy_s(emp1.name, sizeof(emp1.name), "Alexey");
        emp1.hours = 40.0;
        
        Employee emp2;
        emp2.num = 1;
        strcpy_s(emp2.name, sizeof(emp2.name), "Elena");
        emp2.hours = 38.5;
        
        Employee emp3;
        emp3.num = 2;
        strcpy_s(emp3.name, sizeof(emp3.name), "Sergey");
        emp3.hours = 42.0;
        
        employees.push_back(emp1);
        employees.push_back(emp2);
        employees.push_back(emp3);
        
        return employees;
    }

    Employee likeFindEmployee(int employeeId, const std::vector<Employee>& employees) {
        for (const auto& emp : employees) {
            if (emp.num == employeeId) {
                return emp;
            }
        }
        Employee notFound;
        notFound.num = -1;
        return notFound;
    }
    
    bool likeChangeEmployee(const Employee& empData, std::vector<Employee>& employees) {
        for (auto& emp : employees) {
            if (emp.num == empData.num) {
                emp = empData;
                return true;
            }
        }
        return false;
    }
    
    TEST_CASE("FindEmployee functionality") {
        auto employees = createTestEmployees();

        Employee found = likeFindEmployee(1, employees);
        CHECK(found.num == 1);
        CHECK(strcmp(found.name, "Elena") == 0);
        CHECK(found.hours == 38.5);

        Employee notFound = likeFindEmployee(999, employees);
        CHECK(notFound.num == -1);
    }
    
    TEST_CASE("ChangeEmployee functionality") {
        auto employees = createTestEmployees();

        Employee modifiedEmp;
        modifiedEmp.num = 1;
        strcpy_s(modifiedEmp.name, sizeof(modifiedEmp.name), "NewName");
        modifiedEmp.hours = 45.0;
        
        bool success = likeChangeEmployee(modifiedEmp, employees);
        CHECK(success);
        CHECK(employees[1].num == 1);
        CHECK(strcmp(employees[1].name, "NewName") == 0);
        CHECK(employees[1].hours == 45.0);

        Employee nonExistingEmp;
        nonExistingEmp.num = 999;
        strcpy_s(nonExistingEmp.name, sizeof(nonExistingEmp.name), "NoOne");
        nonExistingEmp.hours = 0.0;
        
        success = likeChangeEmployee(nonExistingEmp, employees);
        CHECK_FALSE(success);
    }
}
