from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session, url_for
from flask_session import Session
from passlib.apps import custom_app_context as pwd_context
from tempfile import mkdtemp

from helpers import *

# configure application
app = Flask(__name__)

# ensure responses aren't cached
if app.config["DEBUG"]:
    @app.after_request
    def after_request(response):
        response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
        response.headers["Expires"] = 0
        response.headers["Pragma"] = "no-cache"
        return response

# custom filter
app.jinja_env.filters["usd"] = usd

# configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

@app.route("/")
@login_required
def index():
    rows = db.execute("SELECT stock, sum(amount) as amount FROM deals WHERE user_id = :user_id GROUP BY stock HAVING SUM(amount) != 0", user_id=session["user_id"])
    results = []
    for line in rows:
        stock_info = lookup(line["stock"])
        results.append({
            "stock": stock_info,
            "amount": line["amount"]
        })
    rows = db.execute("SELECT (SELECT cash FROM users WHERE id = :user_id) - SUM(amount * price) as balance FROM deals WHERE user_id = :user_id;", user_id=session["user_id"])
    cash = rows[0]["balance"]
    return render_template("index.html", stocks=results, cash=cash)

@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock."""
    if request.method == "POST":
        # ensure positive number
        try:
            stock_count = int(request.form.get("number-of-shares"))
        except:
            return apology("number provided is not really a number")
        if not stock_count > 0:
            return apology("number of shares should be positive")
            
        # ensure stock provided
        stock_value = request.form.get("stock-value")
        if not stock_value:
            return apology("must provide stock symbol")

        stock_info = lookup(stock_value);
        if not stock_info:
            return apology("unable to find this stock")
        
        rows = db.execute("SELECT * FROM users WHERE id = :id", id=session["user_id"])
        if len(rows) != 1:
            return apology("unable to verify balance")
            
        balance = rows[0]["cash"]
        
        if stock_count * stock_info["price"] > balance:
            return apology("your cannot afford this deal")

        rows = db.execute("INSERT INTO deals(user_id, stock, amount, price) VALUES (:user_id, :stock, :amount, :price)", user_id=session["user_id"], price=stock_info["price"], amount=stock_count, stock=stock_info["symbol"])

        return redirect(url_for("index"))
    else:
        return render_template("buy.html")

@app.route("/history")
@login_required
def history():
    """Show history of transactions."""
    rows = db.execute("SELECT date, stock, amount, price FROM deals WHERE user_id = :user_id ORDER BY date", user_id=session["user_id"])
    return render_template("history.html", rows=rows)

@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in."""

    # forget any user_id
    session.clear()

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username")

        # ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password")

        # query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))

        # ensure username exists and password is correct
        if len(rows) != 1 or not pwd_context.verify(request.form.get("password"), rows[0]["hash"]):
            return apology("invalid username and/or password")

        # remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # redirect user to home page
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")

@app.route("/logout")
def logout():
    """Log user out."""

    # forget any user_id
    session.clear()

    # redirect user to login form
    return redirect(url_for("login"))

@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    if request.method == "POST":
        stockinfo = lookup(request.form.get("search-value"))
        return render_template("quoted.html", stock=stockinfo)
    else:
        return render_template("quote.html")

@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user."""
    if request.method == "POST":

        # ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username")

        # ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password")

        # ensure password match password repeat
        elif not request.form.get("password") == request.form.get("password-repeat"):
            return apology("password and password repeat should match")
            
        # query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))

        # ensure no username exists with same name
        if len(rows) != 0:
            return apology("this username already registered")

        rows = db.execute("INSERT INTO users (username, hash) VALUES (:username, :hash_value);", username=request.form.get("username"), hash_value=pwd_context.hash(request.form.get("password")))
        # remember which user has logged in
        #session["user_id"] = rows[0]["id"]

        # redirect user to home page
        return redirect(url_for("login"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("register.html")

@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock."""
    if request.method == "POST":
        # ensure positive number
        sales = []
        index = 0
        while True:
            stock  = request.form.get("symbol[{}]".format(index))
            if stock == None:
                break;
            try:
                amount = int(request.form.get("amount[{}]".format(index)))
            except:
                return apology("number provided is not really a number")
                
            if amount < 0:
                return apology("number of shares should be positive")
            elif amount > 0:
                # validate amount
                rows = db.execute("SELECT SUM(amount) as amount FROM deals WHERE user_id = :user_id and stock=:stock", user_id=session["user_id"], stock=stock)
                amount_present = rows[0]["amount"]
                if amount_present < amount:
                    return apology("you don't have enough shares to sell")    
                # get price
                stock_info = lookup(stock);        
                if not stock_info:
                    return apology("unable to find stock price")
                sales.append({
                    "stock": stock_info,
                    "amount": amount
                })

            index += 1
            
        # validation done, time to submit changes
        for sale in sales:
            rows = db.execute("INSERT INTO deals(user_id, stock, amount, price) VALUES (:user_id, :stock, :amount, :price)", user_id=session["user_id"], price=sale["stock"]["price"], amount=-sale["amount"], stock=sale["stock"]["symbol"])

        return redirect(url_for("index"))
    else:
        rows = db.execute("SELECT stock, SUM(amount) as amount FROM deals WHERE user_id = :user_id GROUP BY stock HAVING SUM(amount) != 0 ORDER BY stock", user_id=session["user_id"])
        return render_template("sell.html", rows=rows)
