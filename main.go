package main

import (
	"fmt"
	"net/http"

	"github.com/gin-gonic/gin"
)

func main() {
	r := gin.Default()

	r.GET("/", func(c *gin.Context) {
		c.HTML(200, "index.html", gin.H{
			"title": "Home",
		})
	})

	r.GET("/login", func(c *gin.Context) {
		c.HTML(200, "login.html", nil)
	})

	r.POST("/submit-login", func(c *gin.Context) {
		username := c.PostForm("username") // Get form data
		password := c.PostForm("password")

		// Handle the login logic here
		fmt.Printf("Username: %s, Password: %s\n", username, password)

		// Send a response to the client
		c.JSON(http.StatusOK, gin.H{
			"message": "Login data received",
			"user":    username,
		})
	})

	// Set the HTML templates directory
	r.LoadHTMLGlob("templates/*")

	r.Run()
}
