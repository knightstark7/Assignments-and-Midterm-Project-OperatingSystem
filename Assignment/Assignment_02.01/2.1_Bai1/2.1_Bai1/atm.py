import threading
import time

class Bank:
    def __init__(self):
        self.accounts = {"user1": ("1234", 5000000), "user2": ("5678", 10000000)}

    def authenticate(self, username, pin):
        if username in self.accounts:
            if self.accounts[username][0] == pin:
                return True
        return False

    def get_account(self, username):
        return Account(username, self.accounts[username][1])

class Account:
    def __init__(self, username, balance):
        self.username = username
        self.balance = balance

    def get_balance(self):
        return self.balance

    def withdraw(self, amount):
        if amount <= self.balance:
            self.balance -= amount
            return True
        else:
            return False

    def deposit(self, amount):
        self.balance += amount

    def change_pin(self, new_pin):
        self.pin = new_pin

class ATM:
    def __init__(self):
        self.bank = Bank()

    def authenticate(self):
        while True:
            username = input("Nhập tên đăng nhập của bạn: ")
            pin = input("Nhập mã PIN của bạn: ")
            if self.bank.authenticate(username, pin):
                print("Xác thực thành công.")
                return username
            else:
                print("Tên đăng nhập hoặc mã PIN không chính xác. Vui lòng thử lại.")

    def run(self):
        print("Chào mừng đến với máy rút tiền ATM!")
        username = self.authenticate()
        account = self.bank.get_account(username)
        while True:
            print("\nTài khoản của bạn có số dư là:", account.get_balance(), "đồng.")
            choice = input("Bạn muốn làm gì (rút tiền/đổi PIN/nạp tiền/kiểm tra số dư/thoát)? ")
            if choice == "rút tiền":
                amount = int(input("Nhập số tiền cần rút: "))
                if amount > 0:
                    if account.withdraw(amount):
                        print("Bạn đã rút", amount, "đồng thành công.")
                    else:
                        print("Không thể rút số tiền này. Vui lòng kiểm tra lại số dư trong tài khoản.")
                else:
                    print("Số tiền cần rút phải là số nguyên dương.")
            elif choice == "đổi PIN":
                new_pin = input("Nhập mã PIN mới: ")
                account.change_pin(new_pin)
            elif choice == "nạp tiền":
                amount = int(input("Nhập số tiền cần nạp: "))
                if amount > 0:
                    account.deposit(amount)
                else:
                    print("Số tiền cần nạp phải là số nguyên dương.")
            elif choice == "kiểm tra số dư":
                print("Số dư trong tài khoản của bạn là:", account.get_balance(), "đồng.")
            elif choice == "thoát":
                print("Cảm ơn bạn đã sử dụng dịch vụ của chúng tôi!")
                break
            else:
                print("Lựa chọn không hợp lệ. Vui lòng thử lại.")

if __name__ == "__main__":
    atm = ATM()
    atm.run()
    

