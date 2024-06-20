# CG ex2_3D viewing
 <!-- omit in toc -->

### Ryo Matsushita

### s1300041 IT-SPR

---




<details>
  <summary>Full C code here</summary>

```

```

</details>

## Task1 Draw multiple icosahedrons
<details>
  <summary>C code(drawRecursiveIcosahedron) here</summary>

```

static void drawRecursiveIcosahedron(int level) {
    if (level == 0) return;
    drawIcosahedron();

    // Prepare for the next
    for (int i = 0; i < 2; i++) {
        float num = (i == 0) ? 1.0f : -1.0f;
        glPushMatrix();
        myTranslatef(num * 1.0f, -1.5f, 0.0f);
        myScalef(SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);
        drawRecursiveIcosahedron(level - 1);
        glPopMatrix();
    }
}
```

</details>
You can increase/decrease Icosahedrons by changing recursive level.

### Result(N = 3)

## ![](images/3.png)
<img src="images/3.png" height = "400px">
---

### Result(N = 5)

<img src="images/5.png" height = "400px">
---
## Task2 Implement the transformations
Followings are code I implemented instead of relying on the functions provided by the fixed-function pipeling of OpenGL.

- void myTranslatef(GLfloat x, GLfloat y, GLfloat z)
---
<details>
  <summary>C code here</summary>
```
ggg
```
</details>

<img src="images/translate.png" height = "400px">
---

- myRotatef(GLfloat theta, GLfloat kx, GLfloat ky, GLfloat kz)
---
<details>
  <summary>C code here</summary>
```
gg
```
</details>
<img src="images/rotate.png" height = "400px">
---

- void myScalef(GLfloat sx, GLfloat sy, GLfloat sz)
---
<details>
  <summary>C code here</summary>
```
gg
```
</details>
<img src="images/scale.png" height = "400px">
---

- void myOrtho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far)
---
<details>
  <summary>C code here</summary>
```
gg
```
</details>
<img src="images/3.png" height = "400px">
---
- void myScalef(GLfloat sx, GLfloat sy, GLfloat sz)
---
<details>
  <summary>C code here</summary>
```
gg
```
</details>
<img src="images/3.png" height = "400px">
---
